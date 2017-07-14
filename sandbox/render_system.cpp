#include "render_system.h"

#include <algorithm>
#include <GLFW/glfw3.h>

#include <graphics/api.h>
#include <graphics/renderer.h>

#include <scene_events.h>
#include <scene_utils.h>
#include <event_manager.h>
#include <input_manager.h>
#include <log.h>

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

	batcher_.addBufferStorage(storage3d);
	batcher_.addBufferStorage(storage2d);

	cache_.requireComponent<TransformComponent>();
	cache_.requireComponent<ModelComponent>();
}

void RenderSystem::terminate() {
	api_->terminate();
}

void RenderSystem::pushLayerFront(oak::graphics::Renderer *renderer) {
	renderer->setPipeline(&pipeline_);
	layers_.insert(std::begin(layers_), renderer);
}

void RenderSystem::pushLayerBack(oak::graphics::Renderer *renderer) {
	renderer->setPipeline(&pipeline_);
	layers_.push_back(renderer);
}

void RenderSystem::removeLayer(oak::graphics::Renderer& renderer) {
	layers_.erase(std::remove(std::begin(layers_), std::end(layers_), &renderer), std::end(layers_));
}

void RenderSystem::run() {

	auto& ts = oak::getComponentStorage<const TransformComponent>(*scene_);
	auto& ms = oak::getComponentStorage<const ModelComponent>(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityDeactivateEvent>()) {
		//check for meshes to remove
		if (cache_.contains(*scene_, evt.entity)) {
			auto& mc = oak::getComponent<const ModelComponent>(ms, evt.entity);
			for (auto& mesh : mc.model.get().getMeshes()) {
				batcher_.removeMesh(&mesh);
			}
		}
	}

	cache_.update(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityActivateEvent>()) {
		//check for meshes to add
		if (cache_.contains(*scene_, evt.entity)) {
			auto& tc = oak::getComponent<const TransformComponent>(ts, evt.entity);
			auto& mc = oak::getComponent<const ModelComponent>(ms, evt.entity);
			for (auto& mesh : mc.model.get().getMeshes()) {
				batcher_.addMesh(mc.layer, &mc.material.get(), &mesh, tc.transform, mc.region);
			}
		}
	}


	//make batches
	batcher_.run();
	pipeline_.batches = &batcher_.getBatches();

	//render the layers
	for (auto layer : layers_) {
		layer->render(api_);
	}

	//swap buffers
	api_->swap();
	//clear batches
	batches_.clear();

}
