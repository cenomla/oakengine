#include "test_renderer.h"

#include "opengl/gl_renderer.h"
#include "components.h"

namespace oak::graphics {

	TestRenderer::TestRenderer(Scene& scene) : scene_{ &scene }, storage_{ GL_ARRAY_BUFFER } {}

	void TestRenderer::init() {
		renderer_ = new GLRenderer{};
		renderer_->init();

		storage_.create();

		cache_.requireComponent<TransformComponent>();
		cache_.requireComponent<MeshComponent>();
	}

	void TestRenderer::run() {
		batcher_.run(&storage_);

		for (const auto& batch : batcher_.getBatches()) {
			renderer_->render(&storage_, batch);
		}

	}

}