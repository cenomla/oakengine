#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine.h>
#include <window.h>
#include <graphics/opengl/gl_frame_renderer.h>
#include <graphics/opengl/gl_shader.h>
#include <graphics/opengl/gl_buffer.h>
#include <graphics/opengl/gl_vertex_array.h>

#include <GLFW/glfw3.h>

//TODO:
//

constexpr int ms_table[16][4] =
{
	{-1,-1,-1,-1}, //  0: configuration 0
	{ 3, 0,-1,-1}, //  1: configuration 1
	{ 0, 1,-1,-1}, //  2: configuration 1
	{ 3, 1,-1,-1}, //  3: configuration 2
	{ 1, 2,-1,-1}, //  4: configuration 1
	{ 3, 0, 1, 2}, // *5: configuration 3 : Ambiguous case {1,0,3,2}
	{ 0, 2,-1,-1}, //  6: configuration 2
	{ 3, 2,-1,-1}, //  7: configuration 1
	{ 2, 3,-1,-1}, //  8: configuration 1
	{ 2, 0,-1,-1}, //  9: configuration 2
	{ 0, 1, 2, 3}, //*10: configuration 3 : Ambiguous case {0,3,2,1}
	{ 2, 1,-1,-1}, // 11: configuration 1
	{ 1, 3,-1,-1}, // 12: configuration 2
	{ 1, 0,-1,-1}, // 13: configuration 1
	{ 0, 3,-1,-1}, // 14: configuration 1
	{-1,-1,-1,-1}, // 15: configuration 0
};

constexpr int ms_ambiguous[16][4] =
{
	{-1,-1,-1,-1}, // 0: configuration 0
	{-1,-1,-1,-1}, //  1: configuration 1
	{-1,-1,-1,-1}, //  2: configuration 1
	{-1,-1,-1,-1}, //  3: configuration 2
	{-1,-1,-1,-1}, //  4: configuration 1
	{ 1, 0, 3, 2}, // *5: configuration 3 : Ambiguous case {1,0,3,2}
	{-1,-1,-1,-1}, //  6: configuration 2
	{-1,-1,-1,-1}, //  7: configuration 1
	{-1,-1,-1,-1}, //  8: configuration 1
	{-1,-1,-1,-1}, //  9: configuration 2
	{ 0, 3, 2, 1}, //*10: configuration 3 : Ambiguous case {0,3,2,1}
	{-1,-1,-1,-1}, // 11: configuration 1
	{-1,-1,-1,-1}, // 12: configuration 2
	{-1,-1,-1,-1}, // 13: configuration 1
	{-1,-1,-1,-1}, // 14: configuration 1
	{-1,-1,-1,-1}, // 15: configuration 0
};

constexpr int mstomc_vertex_lookup[6][4] =
{	{0, 1, 2, 3},	//0
	{1, 5, 6, 2},	//1
	{5, 4, 7, 6},	//2
	{4, 0, 3, 7},	//3
	{4, 5, 1, 0},	//4
	{3, 2, 6, 7},	//5
};

constexpr int mstomc_edge_lookup[6][4] = // the same as LookUpMBEdgeIDByFaceID
{
	{0,  1,  2,  3},
	{9,  5, 10,  1},
	{4,  7,  6,  5},
	{8,  3, 11,  7},
	{4,  9,  0,  8},
	{2, 10,  6, 11},
};

constexpr glm::vec3 vertex_offsets[] = {
	glm::vec3{ 0.0f, 0.0f, 0.0f },	//0
	glm::vec3{ 1.0f, 0.0f, 0.0f },	//1
	glm::vec3{ 1.0f, 1.0f, 0.0f },	//2
	glm::vec3{ 0.0f, 1.0f, 0.0f },	//3
	glm::vec3{ 0.0f, 0.0f, 1.0f },	//4
	glm::vec3{ 1.0f, 0.0f, 1.0f },	//5
	glm::vec3{ 1.0f, 1.0f, 1.0f },	//6
	glm::vec3{ 0.0f, 1.0f, 1.0f },	//7
};
/*

const glm::vec3 vertex_offsets[] =
{
	// needs to match the vertMap from Dual Contouring impl
	glm::vec3( 0, 0, 0 ),
	glm::vec3( 0, 0, 1 ),
	glm::vec3( 0, 1, 0 ),
	glm::vec3( 0, 1, 1 ),
	glm::vec3( 1, 0, 0 ),
	glm::vec3( 1, 0, 1 ),
	glm::vec3( 1, 1, 0 ),
	glm::vec3( 1, 1, 1 ),
};*/

constexpr int edgevmap[12][2] = {
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 },
	{ 0, 4 },
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 }
};


struct Segment {
	glm::vec3 start;
	glm::vec3 end;
};

struct HermiteData {
	glm::vec3 point;
	glm::vec3 normal;
};

struct Intersects {
	HermiteData start;
	HermiteData end;
};

struct Face {
	Intersects segs[2];
	glm::vec3 sharpFeature[2];
};

struct Cell {
	Face faces[6];
	uint32_t vertices;
	HermiteData edges[12];
};

std::vector<Segment> segments;
Cell cells[32][32][32];

float sdBox(const glm::vec3& p, const glm::vec3& b) {
	glm::vec3 d = glm::abs(p) - b;
	return glm::min(glm::max(d.x,glm::max(d.y,d.z)),0.0f) + glm::length(glm::max(d, 0.0f));
}

float sdTriPrism(const glm::vec3& p, const glm::vec2& h )
{
    glm::vec3 q = glm::abs(p);
    return glm::max(q.z-h.y,glm::max(q.x*0.866025f+p.y*0.5f,-p.y)-h.x*0.5f);
}
float sdHexPrism(const glm::vec3& p, const glm::vec2& h)
{
    glm::vec3 q = glm::abs(p);
    return glm::max(q.z-h.y,glm::max((q.x*0.866025f+q.y*0.5f),q.y)-h.x);
}
float sdCone(const glm::vec3& p, const glm::vec2& c)
{
    // c must be normalized
    float q = glm::length(glm::vec2{p.x, p.y});
    return glm::dot(c,glm::vec2(q,p.z));
}
float sdTorus(const glm::vec3& p, const glm::vec2& t)
{
  glm::vec2 q = glm::vec2(glm::length(glm::vec2{ p.x, p.z }) - t.x, p.y);
  return glm::length(q) - t.y;
}

constexpr glm::vec3 boxPos{ 16.0f };
constexpr glm::vec3 boxSize{ 8.0f };
constexpr glm::vec3 torusPos{ 16.0f };
constexpr glm::vec2 torusSize{ 4.0f, 2.0f };
constexpr glm::vec3 hexPos{ 22.5f, 16.0f, 16.5f };
constexpr glm::vec2 hexSize{ 6.0f, 2.0 };

float sfunc(const glm::vec3& p) {
	return sdTriPrism(-hexPos + p, glm::vec2{ 8.0f, 2.0f });
}

glm::vec3 calc_sfunc_normal(const glm::vec3& p) {
	const float H = 0.001f;
	const float dx = sfunc(p + glm::vec3{ H, 0.0f, 0.0f }) - sfunc(p - glm::vec3{ H, 0.0f, 0.0f });
	const float dy = sfunc(p + glm::vec3{ 0.0f, H, 0.0f }) - sfunc(p - glm::vec3{ 0.0f, H, 0.0f });
	const float dz = sfunc(p + glm::vec3{ 0.0f, 0.0f, H }) - sfunc(p - glm::vec3{ 0.0f, 0.0f, H });

	return glm::normalize(glm::vec3{ dx, dy, dz });
}

glm::vec3 approx_sfunc_zero(const glm::vec3& p0, const glm::vec3& p1) {
	float minValue = 100000.0f;
	float t = 0.0f;
	float ct = 0.0f;
	const float increment = 1.0f / 8.0f;
	while (ct <= 1.0f) {
		const glm::vec3 p = p0 + ((p1 - p0) * ct);
		const float density = glm::abs(sfunc(p));
		if (density < minValue) {
			minValue = density;
			t = ct;
		}
		ct += increment;
	}
	return p0 + ((p1 - p0) * t);
}

//fill cells with hermite data
void fillCells() {
	glm::vec3 pos{ 0.0f };
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			for (int k = 0; k < 32; k++) {
				auto& cell = cells[i][j][k];
				pos = { i, j, k };
				//for each vertex sample the sd box function and determine if the vertex is outside (+) or inside(-)
				for (int a = 0; a < 8; a ++) {
					float field = sfunc(pos + vertex_offsets[a]);
					cell.vertices |= (field > 0.0f ? 1 : 0) << a;
				}
				//TODO: proper placement of edge positions, normal calculation
				for (int a = 0; a < 12; a++) {
					const int c0 = edgevmap[a][0];
					const int c1 = edgevmap[a][1];

					if (((cell.vertices >> c0) & 1) && ((cell.vertices >> c1) & 1)) {
						continue;
					}

					const glm::vec3 p = approx_sfunc_zero(pos + vertex_offsets[c0], pos + vertex_offsets[c1]);
					cell.edges[a] = { p, calc_sfunc_normal(p) };
				}
			}
		}
	}
}

bool isAmbiguous(const Face& face, int index) {
	glm::vec2 si[2];
	for (int i = 0; i < 2; i++) {
		glm::vec2 s0sp, s0sn, s0ep, s0en;
		if (index == 0 || index == 2) {
			s0sp = { face.segs[i].start.point.x, face.segs[i].start.point.y };
			s0sn = { face.segs[i].start.normal.x, face.segs[i].start.normal.y };
			s0ep = { face.segs[i].end.point.x, face.segs[i].end.point.y };
			s0en = { face.segs[i].end.normal.x, face.segs[i].end.normal.y };
		} else if (index == 1 || index == 3) {
			s0sp = { face.segs[i].start.point.z, face.segs[i].start.point.y };
			s0sn = { face.segs[i].start.normal.z, face.segs[i].start.normal.y };
			s0ep = { face.segs[i].end.point.z, face.segs[i].end.point.y };
			s0en = { face.segs[i].end.normal.z, face.segs[i].end.normal.y };
		} else if (index == 4 || index == 5) {
			s0sp = { face.segs[i].start.point.x, face.segs[i].start.point.z };
			s0sn = { face.segs[i].start.normal.x, face.segs[i].start.normal.z };
			s0ep = { face.segs[i].end.point.x, face.segs[i].end.point.z };
			s0en = { face.segs[i].end.normal.x, face.segs[i].end.normal.z };
		}
		float s0sm = s0sn.x / -s0sn.y;
		float s0em = s0en.x / -s0en.y;
		float s0sb = s0sp.y - s0sm * s0sp.x;
		float s0eb = s0ep.y - s0em * s0ep.x;

		si[i].x = (s0sb - s0eb) / (s0em - s0sm);
		si[i].y = (s0sm * s0sb - s0sm * s0eb) / (s0em - s0sm) + s0sb;
	}
	//we have intersect!

	//if the points overlap return true
	if (glm::length(si[1] - si[0]) > 0.0f) {
		return true;
	}
	return false;
}

glm::vec3 findSharpFeature(const Face& face, int index) {

	return {};
	
}

//interate over all the cells and spit out line segments
void getSegments() {

	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			for (int k = 0; k < 32; k++) {
				auto& cell = cells[i][j][k];
				//set face pointer
				for (int a = 0; a < 6; a++) {
					auto& face = cell.faces[a];
					int msMask = 0;
					for (int b = 0; b < 4; b++) {
						int vl = mstomc_vertex_lookup[a][b];
						msMask |= (((cell.vertices & (1<<vl)) >> vl) << b);
					}
					const int *msEdges = ms_table[msMask];
					//calculate segments
					if (msEdges[0] != -1) {
						face.segs[0] = { cell.edges[mstomc_edge_lookup[a][msEdges[0]]], cell.edges[mstomc_edge_lookup[a][msEdges[1]]] };

						if (msEdges[2] != -1) {
							face.segs[1] = { cell.edges[mstomc_edge_lookup[a][msEdges[2]]], cell.edges[mstomc_edge_lookup[a][msEdges[3]]] };
							if (isAmbiguous(face, a)) {
								msEdges = ms_ambiguous[msMask];
								face.segs[0] = { cell.edges[mstomc_edge_lookup[a][msEdges[0]]], cell.edges[mstomc_edge_lookup[a][msEdges[1]]] };
								face.segs[1] = { cell.edges[mstomc_edge_lookup[a][msEdges[2]]], cell.edges[mstomc_edge_lookup[a][msEdges[3]]] };
							}
							segments.push_back({ face.segs[1].start.point, face.segs[1].end.point });
						}
						segments.push_back({ face.segs[0].start.point, face.segs[0].end.point });
					}
				}
			}
		}
	}


}

constexpr float PI = 3.14159265358979f;

int main(int argc, char **argv) {

	oak::Engine engine;
	engine.init();

	oak::Window window{ engine, oak::Window::GL_CONTEXT };
	oak::graphics::GLFrameRenderer frameRenderer{ engine };

	engine.addSystem(&window);
	engine.addSystem(&frameRenderer);

	//load opengl stuff
	glfwMakeContextCurrent(window.getWindowHandle());
	glfwSwapInterval(1);

	if (!gladLoadGL()) {
		std::cout << "cannot load gl" << std::endl;
		abort();
	}

	std::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;

	//camera
	glm::vec3 position{ 4.0f, 24.0f, 4.0f };
	glm::vec3 look{ 16.0f, 0.0f, 16.0f };
	float dir = 0.0f, zdir = 0.0f;
	double cursorX = 640.0, cursorY = 360.0;
	float speed = 0.4f;
	GLFWwindow *win = window.getWindowHandle();
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//setup renderer
	oak::graphics::GLShader shader;
	shader.create("res/shaders/voxel/opengl.vert", "res/shaders/voxel/opengl.frag");
	shader.bind();

	glm::mat4 proj = glm::perspective(70.0f * PI / 180.0f, 1280.0f/720.0f, 0.1f, 100.0f);
	shader.setUniform("proj", proj);

	oak::graphics::GLVertexArray vao;
	vao.create();
	vao.bind();
	oak::graphics::GLBuffer vbo{ GL_ARRAY_BUFFER };
	vbo.create();
	vbo.bind();
	vao.attributeDescription(
		{0, 0, sizeof(glm::vec3)}, {
			{0, 0, 3, 0}
		});
	vbo.unbind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glLineWidth(2.0f);
	
	fillCells();


	while (engine.isRunning()) {
		glViewport(0, 0, 1280, 720);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		getSegments();

		vbo.bind();
		vbo.bufferData(segments.size() * 24, segments.data(), GL_STREAM_DRAW);
		vbo.unbind();

		if (glfwGetKey(win, GLFW_KEY_W) != GLFW_RELEASE) {
			position += glm::vec3{ glm::cos(dir * PI / 180.0f) * speed, 0.0f, glm::sin(dir * PI / 180.0f) * speed };
		}
		if (glfwGetKey(win, GLFW_KEY_A) != GLFW_RELEASE) {
			position += glm::vec3{ glm::cos((dir - 90) * PI / 180.0f) * speed, 0.0f, glm::sin((dir - 90) * PI / 180.0f) * speed };
		}
		if (glfwGetKey(win, GLFW_KEY_S) != GLFW_RELEASE) {
			position -= glm::vec3{ glm::cos(dir * PI / 180.0f) * speed, 0.0f, glm::sin(dir * PI / 180.0f) * speed };
		}
		if (glfwGetKey(win, GLFW_KEY_D) != GLFW_RELEASE) {
			position += glm::vec3{ glm::cos((dir + 90) * PI / 180.0f) * speed, 0.0f, glm::sin((dir + 90) * PI / 180.0f) * speed };
		}
		if (glfwGetKey(win, GLFW_KEY_SPACE) != GLFW_RELEASE) {
			position.y += speed;
		}
		if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) {
			position.y -= speed;
		}

		double cx, cy, dx = 0.0, dy = 0.0;
		glfwGetCursorPos(win, &cx, &cy);
		dx = cx - cursorX;
		dy = cy - cursorY;
		cursorX = cx;
		cursorY = cy;

		dir += dx / 6.0f;
		zdir -= dy / 6.0f;
		zdir = glm::min(glm::max(zdir, -80.0f), 80.0f);

		float dist = 10.0f;
		look = position + glm::vec3{ glm::cos(dir * PI / 180.0f) * dist, glm::sin(zdir * PI / 180.0f) * dist, glm::sin(dir * PI / 180.0f) * dist };

		glm::mat4 view = glm::lookAt(position, look, glm::vec3{ 0.0f, 1.0f, 0.0f });
		shader.setUniform("view", view);

		glDrawArrays(GL_LINES, 0, segments.size() * 2);
		segments.clear();

		glfwSwapBuffers(win);
		window.update();
	}
	vao.unbind();
	shader.unbind();

	engine.destroy();

	return 0;
}