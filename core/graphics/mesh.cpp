#include "mesh.h"

#include "util/file_util.h"
#include "util/string_util.h"

namespace oak::graphics {

	Mesh::Mesh(const MeshDescriptor& descriptor) : descriptor_{ descriptor } {}

	void Mesh::load(const oak::string &path) {

		oak::string file = util::readFileAsString(path);
		oak::vector<oak::string> tokens;

		oak::vector<glm::vec3> vertices;
		oak::vector<glm::vec2> uvs;
		oak::vector<glm::vec3> normals;
		oak::vector<glm::ivec3> faces;

		//get vertices and face indices from obj file
		size_t prev = 0, pos;
		do {
			pos = file.find('\n', prev);
			const auto& line = file.substr(prev, pos - prev);
			prev = pos + 1;
			if (line[0] == 'v') {
				if (line[1] == ' ') {
					//parse vertices
					util::splitstr(line, " ", tokens);
					//put vertices data into vertices vector
					vertices.push_back(glm::vec3{ std::stof(tokens[1].c_str()), std::stof(tokens[2].c_str()), std::stof(tokens[3].c_str()) });
					tokens.clear();
				}
				else if (line[1] == 't') {
					//parse texture coords
					util::splitstr(line, " ", tokens);
					//put texcoord data into uvs vector
					uvs.push_back(glm::vec2{ std::stof(tokens[1].c_str()), std::stof(tokens[2].c_str()) });
					tokens.clear();
				}
				else if (line[1] == 'n') {
					//parse normals
					util::splitstr(line, " ", tokens);
					//put normal data into normal vector
					normals.push_back(glm::vec3{ std::stof(tokens[1].c_str()), std::stof(tokens[2].c_str()), std::stof(tokens[3].c_str()) });
					tokens.clear();
				}
			}
			else if (line[0] == 'f') {
				util::splitstr(line, " ", tokens);
				oak::vector<oak::string> indices;
				//parse first point in the face
				for (int i = 1; i <= 3; i++) {
					util::splitstr(tokens[i], "/", indices);
					faces.push_back(glm::ivec3{ std::stoi(indices[0].c_str()), std::stoi(indices[1].c_str()), std::stoi(indices[2].c_str()) });
					indices.clear();
				}
				tokens.clear();
			}
		} while (pos != oak::string::npos);

		for (const auto& face : faces) {
			vertices_.push_back(MeshVertex{ vertices[face.x - 1], normals[face.z - 1], glm::vec2{ uvs[face.y - 1].x, 1.0f - uvs[face.y - 1].y } });
		}

	}

	void Mesh::draw(void *buffer, const glm::mat4& transform) const {
		MeshVertex *data = static_cast<MeshVertex*>(buffer);

		size_t i = 0;
		for (const auto& vertex : vertices_) {
			data[i++] = { glm::vec3{ transform * glm::vec4{ vertex.position, 1.0f } }, vertex.normal, vertex.uvs };
		}
	}

}