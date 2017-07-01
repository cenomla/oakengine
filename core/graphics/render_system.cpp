#include "render_system.h"

#include "opengl/gl_renderer.h"
#include "components.h"

namespace oak::graphics {

	RenderSystem::RenderSystem(Scene& scene, Renderer& renderer) : scene_{ &scene }, renderer_{ &renderer } {}

	void RenderSystem::init() {
		renderer_->init();
		cache_.requireComponent<TransformComponent>();
		cache_.requireComponent<MeshComponent>();

		clearStage_.color = glm::vec4{ 0.3f, 0.5f, 0.9f, 1.0f };
		
		pipeline_.stages.push_back(&clearStage_);
		pipeline_.stages.push_back(&drawStage_);
		pipeline_.stages.push_back(&swapStage_);
		renderer_->setPipeline(&pipeline_);
	}

	void RenderSystem::terminate() {
		renderer_->terminate();
	}

	void RenderSystem::run() {
		if (!renderer_) { return; }

		batcher_.run();

		drawStage_.batches.clear();

		//copy batches from batcher to draw stage
		for (const auto& batch : batcher_.getBatches()) {
			drawStage_.batches.push_back({ 0, batch });
		}

		renderer_->render();

	}

}