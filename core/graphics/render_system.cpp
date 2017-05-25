#include "render_system.h"

#include "opengl/gl_renderer.h"
#include "components.h"

namespace oak::graphics {

	RenderSystem::RenderSystem(Scene& scene, Renderer& renderer) : scene_{ &scene }, renderer_{ &renderer } {}

	void RenderSystem::init() {
		renderer_->init();
		cache_.requireComponent<TransformComponent>();
		cache_.requireComponent<MeshComponent>();
	}

	void RenderSystem::terminate() {
		renderer_->terminate();
	}

	void RenderSystem::run() {
		if (!renderer_) { return; }

		batcher_.run(renderer_->getStorage(), renderer_->getIStorage());

		for (const auto& batch : batcher_.getBatches()) {
			renderer_->render(batch);
		}

		renderer_->swap();

	}

}