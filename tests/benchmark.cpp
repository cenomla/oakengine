#include <glm/glm.hpp>

#include <event_manager.h>
#include <scene_events.h>
#include <input_events.h>
#include <scene.h>
#include <scene_utils.h>
#include <component_storage.h>
#include <entity_id.h>
#include <entity_cache.h>
#include <container.h>
#include <chrono>

struct TransformComponent {
	glm::vec2 position;
	float rotation;
	float scale;
};

struct DrawComponent {
	size_t spriteId;
	uint32_t layer;
	float depth;
};

struct VelocityComponent {
	glm::vec2 velocity;
};

struct BoxComponent {
	glm::vec2 offset;
	glm::vec2 halfExtent;
};

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, DrawComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, VelocityComponent& data, const oak::ObjInfo& info) {}
void pup(oak::Puper& puper, BoxComponent& data, const oak::ObjInfo& info) {}

std::chrono::high_resolution_clock::time_point start;
std::chrono::high_resolution_clock::time_point end;

std::vector<std::pair<std::string, size_t>> times;

void oak_bench() {

	oak::EventManager evtManager;
	
	evtManager.addQueue<oak::EntityCreateEvent>();
	evtManager.addQueue<oak::EntityDestroyEvent>();
	evtManager.addQueue<oak::EntityActivateEvent>();
	evtManager.addQueue<oak::EntityDeactivateEvent>();

	oak::Scene scene;
	oak::ComponentTypeManager chs;

	chs.addHandle<TransformComponent>("transform");
	chs.addHandle<DrawComponent>("draw");
	chs.addHandle<VelocityComponent>("velocity");
	chs.addHandle<BoxComponent>("box");

	oak::ComponentStorage ts{ "transform" };
	oak::ComponentStorage ds{ "draw" };
	oak::ComponentStorage vs{ "velocity" };
	oak::ComponentStorage bs{ "box" };

	scene.addComponentStorage(ts);
	scene.addComponentStorage(ds);
	scene.addComponentStorage(vs);
	scene.addComponentStorage(bs);

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
		oak::addComponent<TransformComponent>(scene, entity, glm::vec2{ 1.0f }, 0.0f, 1.0f);
		if (i % 2 == 0) {
			oak::addComponent<DrawComponent>(scene, entity, 0u, 2u, 15.0f);
		}
		if (i % 2 == 1) {
			oak::addComponent<VelocityComponent>(scene, entity, glm::vec2{ 0.0f });
			oak::addComponent<BoxComponent>(scene, entity, glm::vec2{ 16.0f }, glm::vec2{ 16.0f });
		}
		scene.activateEntity(entity);
	}	

	end = std::chrono::high_resolution_clock::now();
	times.push_back({ "oak_create", std::chrono::nanoseconds{ end - start }.count() });


	//iterate
	start = std::chrono::high_resolution_clock::now();
	float dt = 1.0f/60.0f;
	oak::vector<std::pair<glm::vec2, size_t>> draws;
	for (size_t i = 0; i < 1024; i++) {
		drawCache.update(scene);
		physicsCache.update(scene);

		for (const auto& entity : drawCache.entities()) {
			const auto& tc = *static_cast<const TransformComponent*>(ts.getComponent(entity));
			const auto& dc = *static_cast<const DrawComponent*>(ds.getComponent(entity));
		
			draws.push_back({ tc.position, dc.spriteId });
		}

		for (const auto& entity : physicsCache.entities()) {
			auto& tc = *static_cast<TransformComponent*>(ts.getComponent(entity));
			auto& vc = *static_cast<VelocityComponent*>(vs.getComponent(entity));
			auto& bc = *static_cast<BoxComponent*>(bs.getComponent(entity));

			if (bc.offset.x < bc.halfExtent.y) {
				vc.velocity = glm::vec2{ 0.0f };
			} else {
				bc.offset.x = bc.halfExtent.x + 4.0f;
				bc.offset.y -= 0.02f;
			}
			vc.velocity += glm::vec2{ -0.05f, 0.009f };
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