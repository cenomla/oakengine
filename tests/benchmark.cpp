#include <math.h>
#include <scene_events.h>
#include <input_events.h>
#include <oakengine.h>
#include <entity_id.h>
#include <entity_cache.h>
#include <container.h>
#include <chrono>

struct TransformComponent {
	static const oak::TypeInfo typeInfo;
	oak::Vec2 position;
	float rotation;
	float scale;
};

const oak::TypeInfo TransformComponent::typeInfo = oak::makeComponentInfo<TransformComponent>("transform");

struct DrawComponent {
	static const oak::TypeInfo typeInfo;
	size_t spriteId;
	uint32_t layer;
	float depth;
};

const oak::TypeInfo DrawComponent::typeInfo = oak::makeComponentInfo<DrawComponent>("draw");

struct VelocityComponent {
	static const oak::TypeInfo typeInfo;
	oak::Vec2 velocity;
};

const oak::TypeInfo VelocityComponent::typeInfo = oak::makeComponentInfo<VelocityComponent>("velocity");

struct BoxComponent {
	static const oak::TypeInfo typeInfo;
	oak::Vec2 offset;
	oak::Vec2 halfExtent;
};

const oak::TypeInfo BoxComponent::typeInfo = oak::makeComponentInfo<BoxComponent>("box");

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, DrawComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, VelocityComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, BoxComponent& data, const oak::ObjInfo& info) {}

std::chrono::high_resolution_clock::time_point start;
std::chrono::high_resolution_clock::time_point end;

std::vector<std::pair<std::string, size_t>> times;

void oak_bench() {

	oak::EventManager evtManager;
	
	oak::addEventQueue<oak::EntityCreateEvent>();
	oak::addEventQueue<oak::EntityDestroyEvent>();
	oak::addEventQueue<oak::EntityActivateEvent>();
	oak::addEventQueue<oak::EntityDeactivateEvent>();

	oak::Scene scene;
	oak::ComponentTypeManager ctm;

	ctm.addType<TransformComponent>();
	ctm.addType<DrawComponent>();
	ctm.addType<VelocityComponent>();
	ctm.addType<BoxComponent>();

	scene.init();

	auto& ts = oak::getComponentStorage<TransformComponent>(scene);
	auto& ds = oak::getComponentStorage<DrawComponent>(scene);
	auto& vs = oak::getComponentStorage<VelocityComponent>(scene);
	auto& bs = oak::getComponentStorage<BoxComponent>(scene);


	oak::EntityCache drawCache;
	drawCache.requireComponent<TransformComponent>();
	drawCache.requireComponent<DrawComponent>();
	oak::EntityCache physicsCache;
	physicsCache.requireComponent<TransformComponent>();
	physicsCache.requireComponent<VelocityComponent>();
	physicsCache.requireComponent<BoxComponent>();

	//create 1048576 entities, add components to entities
	start = std::chrono::high_resolution_clock::now();

	oak::EntityId entity;
	for (size_t i = 0; i < 64000; i++) {
		entity = scene.createEntity();
		oak::addComponent<TransformComponent>(entity, scene, oak::Vec2{ 1.0f }, 0.0f, 1.0f);
		if (i % 2 == 0) {
			oak::addComponent<DrawComponent>(entity, scene, 0u, 2u, 15.0f);
		}
		if (i % 2 == 1) {
			oak::addComponent<VelocityComponent>(entity, scene, oak::Vec2{ 0.0f });
			oak::addComponent<BoxComponent>(entity, scene, oak::Vec2{ 16.0f }, oak::Vec2{ 16.0f });
		}
		scene.activateEntity(entity);
	}	

	end = std::chrono::high_resolution_clock::now();
	times.push_back({ "oak_create", std::chrono::nanoseconds{ end - start }.count() });


	//iterate
	start = std::chrono::high_resolution_clock::now();
	float dt = 1.0f/60.0f;
	oak::vector<std::pair<oak::Vec2, size_t>> draws;
	for (size_t i = 0; i < 1024; i++) {
		drawCache.update(scene);
		physicsCache.update(scene);

		for (const auto& entity : drawCache.entities()) {
			auto [tc, dc] = oak::getComponents<const TransformComponent, const DrawComponent>(entity, ts, ds);
		
			draws.push_back({ tc.position, dc.spriteId });
		}

		for (const auto& entity : physicsCache.entities()) {
			auto [tc, vc, bc] = oak::getComponents<TransformComponent, VelocityComponent, BoxComponent>(entity, ts, vs, bs);

			if (bc.offset.x < bc.halfExtent.y) {
				vc.velocity = oak::Vec2{ 0.0f };
			} else {
				bc.offset.x = bc.halfExtent.x + 4.0f;
				bc.offset.y -= 0.02f;
			}
			vc.velocity += oak::Vec2{ -0.05f, 0.009f };
			tc.position += vc.velocity * dt;
		}

		draws.clear();
		evtManager.clear();
	}
	end = std::chrono::high_resolution_clock::now();
	times.push_back({ "oak_iterate", std::chrono::nanoseconds{ end - start }.count() / 1024 });
	
	scene.reset();

}

int main(int argc, char **argv) {

	oak_bench();

	for (auto& t : times) {
		printf("%s: %lins\n", t.first.c_str(), t.second);
	}

	times.clear();

	return 0;

}
