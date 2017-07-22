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

	auto& layoutMesh = lrs.add("mesh", 
	oak::vector<oak::graphics::AttributeType>{
		oak::graphics::AttributeType::POSITION,
		oak::graphics::AttributeType::NORMAL,
		oak::graphics::AttributeType::UV
	});

	auto& layoutSprite = lrs.add("sprite",
	oak::vector<oak::graphics::AttributeType>{
		oak::graphics::AttributeType::POSITION2D,
		oak::graphics::AttributeType::UV
	});

	auto& layoutParticle = lrs.add("particle", 
	oak::vector<oak::graphics::AttributeType> {
		oak::graphics::AttributeType::POSITION,
		oak::graphics::AttributeType::NORMAL,
		oak::graphics::AttributeType::UV,
		oak::graphics::AttributeType::INSTANCE_POSITION
	});

	storageMesh_.create(&layoutMesh);
	storageSprite_.create(&layoutSprite);
	storageParticle_.create(&layoutParticle);

	meshBatcher_.setBufferStorage(&storageMesh_);
	spriteBatcher_.setBufferStorage(&storageSprite_);
	particleSystem_.setBufferStorage(&storageParticle_);

	meshCache_.requireComponent<TransformComponent>();
	meshCache_.requireComponent<MeshComponent>();

	spriteCache_.requireComponent<Transform2dComponent>();
	spriteCache_.requireComponent<SpriteComponent>();

	particleCache_.requirePrefab(std::hash<oak::string>{}("particle"));
	particleCache_.requireComponent<MeshComponent>();

	pipeline_.batches = &batches_;
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
	auto& t2s = oak::getComponentStorage<const Transform2dComponent>(*scene_);
	auto& ms = oak::getComponentStorage<const MeshComponent>(*scene_);
	auto& ss = oak::getComponentStorage<const SpriteComponent>(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityDeactivateEvent>()) {
		//check for meshes to remove
		if (meshCache_.contains(*scene_, evt.entity)) {
			auto& mc = oak::getComponent<const MeshComponent>(ms, evt.entity);
			meshBatcher_.removeMesh(mc.mesh);
		}
	}

	meshCache_.update(*scene_);
	spriteCache_.update(*scene_);
	particleCache_.update(*scene_);

	for (const auto& evt : oak::EventManager::inst().getQueue<oak::EntityActivateEvent>()) {
		//check for meshes to add
		if (meshCache_.contains(*scene_, evt.entity)) {
			auto& tc = oak::getComponent<const TransformComponent>(ts, evt.entity);
			auto& mc = oak::getComponent<const MeshComponent>(ms, evt.entity);
			meshBatcher_.addMesh(mc.layer, mc.material, mc.mesh, tc.transform, mc.region);
		}
		//check for particles to add
		if (particleCache_.contains(*scene_, evt.entity)) {
			auto& mc = oak::getComponent<const MeshComponent>(ms, evt.entity);
			particleSystem_.setMesh(mc.layer, mc.material, mc.mesh, mc.region);
		}
	}

	for (const auto& entity : spriteCache_.entities()) {
		auto& t2c = oak::getComponent<const Transform2dComponent>(t2s, entity);
		auto& sc = oak::getComponent<const SpriteComponent>(ss, entity);
		spriteBatcher_.addSprite(sc.layer, sc.material, sc.sprite, t2c.transform);
	}


	//make batches
	meshBatcher_.run();
	spriteBatcher_.run();
	particleSystem_.run();
	batches_.insert(std::end(batches_), std::begin(meshBatcher_.getBatches()), std::end(meshBatcher_.getBatches()));
	batches_.insert(std::end(batches_), std::begin(spriteBatcher_.getBatches()), std::end(spriteBatcher_.getBatches()));
	batches_.push_back(particleSystem_.getBatch());


	//render the layers
	for (auto layer : layers_) {
		layer->render(api_);
	}

	//swap buffers
	api_->swap();
	//clear batches
	batches_.clear();

}
