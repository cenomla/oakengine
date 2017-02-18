#include "light_renderer.h"

#include <components.h>
#include <view_system.h>
#include <engine.h>

#include "component_ext.h"

LightRenderer::LightRenderer(oak::Engine &engine) : oak::System{ engine, "light_renderer" }, vbo_{ GL_ARRAY_BUFFER }, tex_{ GL_TEXTURE_2D }, fvbo_{ GL_ARRAY_BUFFER } {


}

LightRenderer::~LightRenderer() {

}

void LightRenderer::init() {
	shadowCache_.requireComponent<oak::TransformComponent>();
	shadowCache_.requireComponent<oak::AABB2dComponent>();
	shadowCache_.requireComponent<OccluderComponent>();
	lightCache_.requireComponent<oak::TransformComponent>();
	lightCache_.requireComponent<LightComponent>();

	engine_.getSystem<oak::EntityManager>().addCache(&shadowCache_);
	engine_.getSystem<oak::EntityManager>().addCache(&lightCache_);
	engine_.getSystem<oak::graphics::GLRenderer>().setDrawOp(1, [this](auto&&... args) { this->render(std::forward<decltype(args)>(args)...); });

	//setup rendering stuff
	vao_.create();
	vao_.bind();

	vbo_.create();
	vbo_.bind();
	vao_.attributeDescription(
		{0, 0, sizeof(glm::vec2)}, {
			{0, 0, 2, 0}
		});
	
	vbo_.unbind();
	vao_.unbind();

	tex_.create(1280, 720, nullptr);
	fbo_.create(tex_, 1280, 720);
	shadowShader_.create("res/shaders/shadow/opengl.vert", "res/shaders/shadow/opengl.frag");

	fvao_.create();
	fvao_.bind();

	fvbo_.create();
	fvbo_.bind();
	fvao_.attributeDescription({ 0, 0, sizeof(glm::vec2) }, { { 0, 0, 2, 0} });

	float data[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f,
		-1.0f, -1.0f
	};

	fvbo_.bufferData(sizeof(data), data, GL_STATIC_DRAW);

	fvbo_.unbind();
	fvao_.unbind();
	fullscreenShader_.create("core/graphics/shaders/fullscreen/opengl.vert", "core/graphics/shaders/fullscreen/opengl.frag");
}

void LightRenderer::update() {
	auto &renderer = engine_.getSystem<oak::graphics::GLRenderer>();

	//generate shadows
	const auto& lights = lightCache_.entities();
	for (size_t i = 0; i < lights.size(); i++) {
		const auto &entity = lights[i];
		const auto& tc = entity.getComponent<oak::TransformComponent>();
		const auto& lc = entity.getComponent<LightComponent>();

		renderer.addObject(glm::vec2{ tc.position }, tc.position.z + i, 1, tc.rotationAngle, tc.scale, &lc);
	}
}

void LightRenderer::render(const oak::graphics::GLVertexArray& vao, const oak::graphics::Batch& batch) {
	fbo_.bind();
	glViewport(0, 0, 1280, 720);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);//additive blending

	auto& viewSystem = engine_.getSystem<oak::ViewSystem>();

	glm::vec2 pos;

	for (const auto& shadow : shadowCache_.entities()) {
		const auto& stc = shadow.getComponent<oak::TransformComponent>();
		const auto& saabb = shadow.getComponent<oak::AABB2dComponent>();

		pos = glm::vec2{ stc.position } + saabb.offset;

		lines_.push_back({ 
			glm::vec2{ pos.x + saabb.halfExtent.x, pos.y + saabb.halfExtent.y }, 
			glm::vec2{ pos.x - saabb.halfExtent.x, pos.y + saabb.halfExtent.y } 
		});

		lines_.push_back({ 
			glm::vec2{ pos.x + saabb.halfExtent.x, pos.y - saabb.halfExtent.y }, 
			glm::vec2{ pos.x + saabb.halfExtent.x, pos.y + saabb.halfExtent.y } 
		});

		lines_.push_back({ 
			glm::vec2{ pos.x - saabb.halfExtent.x, pos.y - saabb.halfExtent.y }, 
			glm::vec2{ pos.x + saabb.halfExtent.x, pos.y - saabb.halfExtent.y }
		});

		lines_.push_back({ 
			glm::vec2{ pos.x - saabb.halfExtent.x, pos.y + saabb.halfExtent.y },
			glm::vec2{ pos.x - saabb.halfExtent.x, pos.y - saabb.halfExtent.y }
		});
	}

	const auto& lights = lightCache_.entities();
	for (size_t i = 0; i < batch.objectCount; i++) {
		const auto& light = lights[i];
		const auto& ltc = light.getComponent<oak::TransformComponent>();
		const auto& llc = light.getComponent<LightComponent>();

		vbo_.bind();
		vbo_.bufferData((shadowCache_.entities().size()) * 192 , nullptr, GL_STREAM_DRAW);
		glm::vec2 *buffer = static_cast<glm::vec2*>(vbo_.map(GL_WRITE_ONLY));
		size_t vertexCount = 0;

		glm::vec2 normal, lightToStart, lightToEnd, pos;

		for (const auto& line : lines_) {
			normal = glm::normalize(glm::vec2{ -(line.start.y - line.end.y), line.start.x - line.end.x });
			lightToStart = glm::normalize(line.start - glm::vec2{ ltc.position });
			if (glm::dot(normal, lightToStart) > 0.0f) {
				lightToEnd = glm::normalize(line.end - glm::vec2{ ltc.position });

				buffer[0] = line.start;
				buffer[1] = line.end;
				buffer[2] = glm::vec2{ line.end + lightToEnd * llc.size };
				buffer[3] = buffer[2];
				buffer[4] = glm::vec2{ line.start + lightToStart * llc.size };
				buffer[5] = buffer[0];

				buffer += 6;
				vertexCount += 6;
			}
		}

		vbo_.unmap();
		vbo_.unbind();

		//render shadows
		glColorMask(false, false, false, false);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		//draw code goes here

		shadowShader_.bindBlockIndex("MatrixBlock", viewSystem.getViewId(batch.layer));
		shadowShader_.bind();
		vao_.bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		
		glColorMask(true, true, true, true);


		//render light
		glStencilFunc(GL_EQUAL, 0, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilMask(0xFF);

		vao.bind();
		batch.material->shader->bindBlockIndex("MatrixBlock", viewSystem.getViewId(batch.layer));
		batch.material->bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void*>((batch.start + i) * 24));
		
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	glDisable(GL_STENCIL_TEST);


	//draw the fbo tex 
	fbo_.unbind();
	glViewport(0, 0, 1280, 720);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);//multiplicative blending

	//blur the texture
	fullscreenShader_.bind();
	tex_.bind(GL_TEXTURE0);
	fvao_.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	lines_.clear();
}