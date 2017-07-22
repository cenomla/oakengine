#include "render_system.h"

#include <algorithm>
#include <GLFW/glfw3.h>

#include <graphics/api.h>
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

	auto& lrs = oak::ResourceManager::inst().get<oak::graphics::AttributeLayout>();

	auto& layout3d = lrs.add("layout3d", 
	oak::vector<oak::graphics::AttributeType>{
		oak::graphics::AttributeType::POSITION,
		oak::graphics::AttributeType::NORMAL,
		oak::graphics::AttributeType::UV
	});

	auto& layoutParticle = lrs.add("layoutParticle", 
	oak::vector<oak::graphics::AttributeType> {
		oak::graphics::AttributeType::POSITION,
		oak::graphics::AttributeType::NORMAL,
		oak::graphics::AttributeType::UV,
		oak::graphics::AttributeType::INSTANCE_POSITION
	});

	storage3d_.create(&layout3d);
	storageParticle_.create(&layoutParticle);

	batcher_.setBufferStorage(&storage3d_);
	particleSystem_.setBufferStorage(&storageParticle_);

	meshCache_.requireComponent<TransformComponent>();
	meshCache_.requireComponent<MeshComponent>();

	particleCache_.requirePrefab(std::hash<oak::string>{}("particle"));
	particleCache_.requireComponent<MeshComponent>();
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
	auto& ms = oak::getComponentStorage<const MeshComponent>(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityDeactivateEvent>()) {
		//check for meshes to remove
		if (meshCache_.contains(*scene_, evt.entity)) {
			auto& mc = oak::getComponent<const MeshComponent>(ms, evt.entity);
			batcher_.removeMesh(mc.mesh);
		}
	}

	meshCache_.update(*scene_);
	particleCache_.update(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityActivateEvent>()) {
		//check for meshes to add
		if (meshCache_.contains(*scene_, evt.entity)) {
			auto& tc = oak::getComponent<const TransformComponent>(ts, evt.entity);
			auto& mc = oak::getComponent<const MeshComponent>(ms, evt.entity);
			batcher_.addMesh(mc.layer, mc.material, mc.mesh, tc.transform, mc.region);
		}
		//check for particles to add
		if (particleCache_.contains(*scene_, evt.entity)) {
			auto& mc = oak::getComponent<const MeshComponent>(ms, evt.entity);
			particleSystem_.setMesh(mc.layer, mc.material, mc.mesh, mc.region);
		}
	}


	//make batches
	oak::vector<oak::graphics::Batch> batches;
	pipeline_.batches = &batches;
	batcher_.run();
	particleSystem_.run();
	batches.insert(std::end(batches), std::begin(batcher_.getBatches()), std::end(batcher_.getBatches()));
	batches.push_back(particleSystem_.getBatch());


	//render the layers
	for (auto layer : layers_) {
		layer->render(api_);
	}

	//swap buffers
	api_->swap();
	//clear batches
	batches_.clear();

}
