#include "light_renderer.h"

#include <system_manager.h>
#include <view_system.h>
#include <component_storage.h>
#include <components.h>

#include "component_ext.h"

LightRenderer::LightRenderer(oak::Scene& scene) : 
vbo_{ GL_ARRAY_BUFFER }, tex_{ GL_TEXTURE_2D }, fvbo_{ GL_ARRAY_BUFFER }, scene_{ &scene } {}

void LightRenderer::init() {
	shadowCache_.requireComponent<oak::TransformComponent>();
	shadowCache_.requireComponent<oak::AABB2dComponent>();
	shadowCache_.requireComponent<OccluderComponent>();
	lightCache_.requireComponent<oak::TransformComponent>();
	lightCache_.requireComponent<LightComponent>();

	oak::SystemManager::inst().getSystem<oak::graphics::GLRenderer>().setDrawOp(1, [this](auto&&... args) { this->render(std::forward<decltype(args)>(args)...); });

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

	tex_.create(320, 180, nullptr);
	fbo_.create(tex_, 320, 180);
	shadowShader_.create("sandbox/res/shaders/shadow/opengl.vert", "sandbox/res/shaders/shadow/opengl.frag");

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

void LightRenderer::run() {
	auto &renderer = oak::SystemManager::inst().getSystem<oak::graphics::GLRenderer>();

	shadowCache_.update(*scene_);
	lightCache_.update(*scene_);

	//generate shadows
	const auto& lights = lightCache_.entities();
	oak::ComponentStorage& ts = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, oak::TransformComponent>::id);
	oak::ComponentStorage& ls = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, LightComponent>::id);
	for (size_t i = 0; i < lights.size(); i++) {
		const auto& entity = lights[i];
		const auto& tc = *static_cast<const oak::TransformComponent*>(ts.getComponent(entity));
		const auto& lc = *static_cast<const LightComponent*>(ls.getComponent(entity));

		renderer.addObject(glm::vec2{ tc.position }, tc.position.z + i, 1, tc.rotationAngle, tc.scale, &lc);
	}
}

void LightRenderer::render(const oak::graphics::GLVertexArray& vao, const oak::graphics::Batch& batch) {
	fbo_.bind();
	glViewport(0, 0, 320, 180);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);//additive blending

	auto& viewSystem = oak::SystemManager::inst().getSystem<oak::ViewSystem>();

	glm::vec2 pos;

	oak::ComponentStorage& ts = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, oak::TransformComponent>::id);
	oak::ComponentStorage& as = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, oak::AABB2dComponent>::id);
	oak::ComponentStorage& ls = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, LightComponent>::id);
	for (const auto& shadow : shadowCache_.entities()) {
		const auto& stc = *static_cast<const oak::TransformComponent*>(ts.getComponent(shadow));
		const auto& saabb = *static_cast<const oak::AABB2dComponent*>(as.getComponent(shadow));

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
		const auto& ltc = *static_cast<const oak::TransformComponent*>(ts.getComponent(light));
		const auto& llc = *static_cast<const LightComponent*>(ls.getComponent(light));

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
				buffer[2] = glm::vec2{ line.end + lightToEnd * llc.size * 2.0f };
				buffer[3] = buffer[2];
				buffer[4] = glm::vec2{ line.start + lightToStart * llc.size * 2.0f };
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