#include "render_system.h"

#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <graphics/gl_api.h>
#include <graphics/renderer.h>

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

	pipeline_.x = 0;
	pipeline_.y = 0;
	pipeline_.width = frameWidth;
	pipeline_.height = frameHeight;

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

	meshCache_.requireComponent<TransformComponent>();
	meshCache_.requireComponent<MeshComponent>();

	pipeline_.batches[0] = &batches_;
}

void RenderSystem::terminate() {
	oak::graphics::shader::destroy(shader_);
	storageMesh_.destroy();
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
	auto& ms = oak::getComponentStorage<const MeshComponent>(*scene_);

	meshCache_.update(*scene_);

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

	//render the layers
	for (auto layer : layers_) {
		layer->render(api_);
	}

	//swap buffers
	api_->swap();
	//clear batches
	batches_.clear();

}
