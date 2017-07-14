#include "render_system.h"

#include <algorithm>
#include <GLFW/glfw3.h>

#include "api.h"
#include "renderer.h"

#include "input_manager.h"

#include "log.h"

namespace oak::graphics {

	RenderSystem::RenderSystem(Scene *scene, Api *api) : scene_{ scene }, api_{ api } {}

	void RenderSystem::init() {
		api_->init();

		int frameWidth, frameHeight;
		glfwGetFramebufferSize(oak::InputManager::inst().getWindow(), &frameWidth, &frameHeight);

		log_print_out("frame width: %i, frame height: %i", frameWidth, frameHeight);

		pipeline_.x = 0;
		pipeline_.y = 0;
		pipeline_.width = frameWidth;
		pipeline_.height = frameHeight;
	}

	void RenderSystem::terminate() {
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
		//make batches
		batcher_.run();
		particleSystem_.run();
		auto& sb = batcher_.getBatches();
		batches_.insert(std::end(batches_), std::begin(sb), std::end(sb));
		batches_.push_back(particleSystem_.getBatch());
		pipeline_.batches = &batches_;

		//render the layers

		for (auto layer : layers_) {
			layer->render(api_);
		}

		//swap buffers
		api_->swap();

		batches_.clear();

	}

}