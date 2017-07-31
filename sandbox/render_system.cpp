#include "render_system.h"

#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <graphics/api.h>

#include <core_components.h>
#include <scene_events.h>
#include <scene_utils.h>
#include <event_manager.h>
#include <input_manager.h>
#include <resource_manager.h>
#include <log.h>

#include "renderer.h"
#include "components.h"

RenderSystem::RenderSystem(oak::Scene *scene, oak::graphics::Api *api) : scene_{ scene }, api_{ api } {}

void RenderSystem::init() {
	api_->init();

	int frameWidth, frameHeight;
	glfwGetFramebufferSize(oak::InputManager::inst().getWindow(), &frameWidth, &frameHeight);

	log_print_out("frame width: %i, frame height: %i", frameWidth, frameHeight);

	pipeline_.x = 0;
	pipeline_.y = 0;
	pipeline_.width = frameWidth;
	pipeline_.height = frameHeight;

	meshBatcher_.init();
	spriteBatcher_.init();
	particleSystem_.init();

	meshCache_.requireComponent<TransformComponent>();
	meshCache_.requireComponent<MeshComponent>();

	spriteCache_.requireComponent<Transform2dComponent>();
	spriteCache_.requireComponent<SpriteComponent>();

	textCache_.requireComponent<Transform2dComponent>();
	textCache_.requireComponent<TextComponent>();

	particleCache_.requirePrefab(std::hash<oak::string>{}("particle"));
	particleCache_.requireComponent<MeshComponent>();
}

void RenderSystem::terminate() {
	meshBatcher_.terminate();
	spriteBatcher_.terminate();
	particleSystem_.terminate();
	api_->terminate();
}

void RenderSystem::pushLayerFront(Renderer *renderer) {
	renderer->setPipeline(&pipeline_);
	layers_.insert(std::begin(layers_), renderer);
}

void RenderSystem::pushLayerBack(Renderer *renderer) {
	renderer->setPipeline(&pipeline_);
	layers_.push_back(renderer);
}

void RenderSystem::removeLayer(Renderer& renderer) {
	layers_.erase(std::remove(std::begin(layers_), std::end(layers_), &renderer), std::end(layers_));
}

void RenderSystem::run() {

	auto& ts = oak::getComponentStorage<const TransformComponent>(*scene_);
	auto& t2s = oak::getComponentStorage<const Transform2dComponent>(*scene_);
	auto& ms = oak::getComponentStorage<const MeshComponent>(*scene_);
	auto& ss = oak::getComponentStorage<const SpriteComponent>(*scene_);
	auto& txs = oak::getComponentStorage<const TextComponent>(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityDeactivateEvent>()) {
		//check for meshes to remove
		if (meshCache_.contains(evt.entity)) {
			auto& mc = oak::getComponent<const MeshComponent>(evt.entity, ms);
			meshBatcher_.removeMesh(mc.mesh);
		}
	}

	meshCache_.update(*scene_);
	spriteCache_.update(*scene_);
	textCache_.update(*scene_);
	particleCache_.update(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityActivateEvent>()) {
		//check for meshes to add
		if (meshCache_.contains(evt.entity)) {
			auto [tc, mc] = oak::getComponents<const TransformComponent, const MeshComponent>(evt.entity, ts, ms);
			meshBatcher_.addMesh(mc.layer, mc.material, mc.mesh, tc.transform, mc.region);
		}
		//check for particles to add
		if (particleCache_.contains(evt.entity)) {
			auto& mc = oak::getComponent<const MeshComponent>(evt.entity, ms);
			particleSystem_.setMesh(mc.layer, mc.material, mc.mesh, mc.region);
		}
	}

	for (const auto& entity : spriteCache_.entities()) {
		auto [tc, sc] = oak::getComponents<const Transform2dComponent, const SpriteComponent>(entity, t2s, ss);
		spriteBatcher_.addSprite(sc.layer, sc.material, sc.sprite, tc.transform);
	}

	for (const auto& entity : textCache_.entities()) {
		auto [t2c, txc] = oak::getComponents<const Transform2dComponent, const TextComponent>(entity, t2s, txs);

		glm::vec2 pos{ 0.0f };
		for (const auto& c : txc.text) {
			auto& glyph = txc.font->glyphs[c];
			spriteBatcher_.addSprite(txc.layer, txc.material, &glyph.sprite, glm::translate(t2c.transform, pos));
			pos.x += glyph.advance;
			if (c == '\n') {
				pos.y += txc.font->size;
			}
		}
	}


	//make batches
	meshBatcher_.run();
	spriteBatcher_.run();
	particleSystem_.run();

	pipeline_.batches[0] = &meshBatcher_.getBatches();
	pipeline_.batches[1] = &spriteBatcher_.getBatches();

	//render the layers
	for (auto layer : layers_) {
		layer->render(api_);
	}

	//swap buffers
	api_->swap();

}
