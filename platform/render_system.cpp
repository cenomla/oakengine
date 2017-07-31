#include "render_system.h"

#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <graphics/gl_api.h>

#include <core_components.h>
#include <scene_events.h>
#include <scene_utils.h>
#include <event_manager.h>
#include <input_manager.h>
#include <resource_manager.h>
#include <log.h>

#include "components.h"

RenderSystem::RenderSystem(oak::Scene *scene, oak::graphics::Api *api) : scene_{ scene }, api_{ api } {}

void RenderSystem::init() {
	api_->init();

	int frameWidth, frameHeight;
	glfwGetFramebufferSize(oak::InputManager::inst().getWindow(), &frameWidth, &frameHeight);

	log_print_out("frame width: %i, frame height: %i", frameWidth, frameHeight);

	storageMesh_.create({
		oak::vector<oak::graphics::AttributeType> {
			oak::graphics::AttributeType::POSITION2D,
			oak::graphics::AttributeType::COLOR
		}
	});

	//load shader
	oak::graphics::ShaderInfo shaderInfo;
	shaderInfo.vertex = "/res/shaders/poly_shader/vert.glsl";
	shaderInfo.fragment = "/res/shaders/poly_shader/frag.glsl";
	shader_ = oak::graphics::shader::create(shaderInfo);

	material_.shader = &shader_;

	state_.clearColor = glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f };
	state_.viewport = { 0, 0, frameWidth, frameHeight };
	state_.blendMode = oak::graphics::BlendMode::NORMAL;

	batcher_.init();

	meshCache_.requireComponent<TransformComponent>();
	meshCache_.requireComponent<MeshComponent>();

	textCache_.requireComponent<TransformComponent>();
	textCache_.requireComponent<TextComponent>();
}

void RenderSystem::terminate() {
	oak::graphics::shader::destroy(shader_);
	storageMesh_.destroy();
	batcher_.terminate();
	api_->terminate();
}

void RenderSystem::run() {

	meshCache_.update(*scene_);
	textCache_.update(*scene_);

	auto& ts = oak::getComponentStorage<const TransformComponent>(*scene_);
	auto& ms = oak::getComponentStorage<const MeshComponent>(*scene_);
	auto& txs = oak::getComponentStorage<const TextComponent>(*scene_);

	//batch sprites
	for (const auto& entity : textCache_.entities()) {
		auto [tc, txc] = oak::getComponents<const TransformComponent, const TextComponent>(entity, ts, txs);
		
		glm::vec2 pos{ 0.0f };
		for (const auto& c : txc.text) {
			auto& glyph = txc.font->glyphs[c];			
			batcher_.addSprite(txc.layer, txc.material, &glyph.sprite, glm::translate(tc.transform, pos));
			pos.x += glyph.advance;
			if (c == '\n') {
				pos.y += txc.font->size;
			}
		}
	}

	struct Vertex {
		glm::vec2 pos;
		unsigned char color[4];
	};
	oak::vector<Vertex> data;
	size_t offset = 0, count = 0;

	for (const auto& entity : meshCache_.entities()) {
		auto [tc, mc] = oak::getComponents<const TransformComponent, const MeshComponent>(entity, ts, ms);

		for (const auto& v : mc.mesh->vertices) {
			data.push_back({ glm::vec2{ tc.transform * glm::vec3{ v.position, 1.0f } }, { 0, 155, 155, 255 } });
			count++;
		}
		batches_.push_back({ &storageMesh_, &material_, offset, count, 0, oak::graphics::Batch::DRAW_LINE_LOOP, -1 });
		offset += count;
		count = 0;
	}
	
	storageMesh_.data(0, data.size() * sizeof(Vertex), data.data());

	batcher_.run();

	//render the things
	api_->setState(state_);
	api_->clear(true);

	for (const auto& batch : batches_) {
		api_->draw(batch);
	}

	for (const auto& batch : batcher_.getBatches()) {
		api_->draw(batch);
	}

	oak::graphics::GLVertexArray::unbind();

	//swap buffers
	api_->swap();
	//clear batches
	batches_.clear();
}
