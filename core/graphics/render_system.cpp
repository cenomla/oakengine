#include "render_system.h"

#include <algorithm>

#include "api.h"
#include "renderer.h"

#include "components.h"

namespace oak::graphics {

	RenderSystem::RenderSystem(Scene& scene, Api& api) : scene_{ &scene }, api_{ &api } {}

	void RenderSystem::init() {
		api_->init();

		cache_.requireComponent<TransformComponent>();
		cache_.requireComponent<MeshComponent>();

		pipeline_.x = 0;
		pipeline_.y = 0;
		pipeline_.width = 1280;
		pipeline_.height = 720;

	}

	void RenderSystem::terminate() {
		api_->terminate();
	}

	void RenderSystem::pushLayerFront(Renderer& renderer) {
		renderer.setPipeline(&pipeline_);
		layers_.insert(std::begin(layers_), &renderer);
	}

	void RenderSystem::pushLayerBack(Renderer& renderer) {
		renderer.setPipeline(&pipeline_);
		layers_.push_back(&renderer);
	}

	void RenderSystem::removeLayer(Renderer& renderer) {
		layers_.erase(std::remove(std::begin(layers_), std::end(layers_), &renderer), std::end(layers_));
	}

	void RenderSystem::run() {
		//make batches
		batcher_.run();
		pipeline_.batches = &batcher_.getBatches();

		//render the layers

		for (auto layer : layers_) {
			layer->render(api_);
		}

		//swap buffers
		api_->swap();

	}

}