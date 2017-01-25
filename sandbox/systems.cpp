#include "systems.h"

#include <glm/glm.hpp>
#include <graphics/opengl/gl_renderer.h>
#include <components.h>
#include <events.h>

#include "tile_system.h"
#include "events.h"

SpriteSystem::SpriteSystem(oak::Engine &engine) : oak::System{ engine, "sprite_system" } {}

void SpriteSystem::init() {
	cache_.requireComponent<oak::TransformComponent>();
	cache_.requireComponent<oak::SpriteComponent>();
	engine_.getSystem<oak::EntityManager>().addCache(&cache_);
}

void SpriteSystem::update() {
	auto &renderer = engine_.getSystem<oak::graphics::GLRenderer>();
	for (const auto& entity : cache_.entities()) {
		const auto &tc = entity.getComponent<oak::TransformComponent>();
		const auto &sc = entity.getComponent<oak::SpriteComponent>();
		renderer.addObject(glm::vec2{ tc.position }, tc.position.z, entity.layer(), tc.rotationAngle, tc.scale, &sc);
	}
}

TextSystem::TextSystem(oak::Engine &engine) : oak::System{ engine, "text_system" } {}

void TextSystem::init() {
	cache_.requireComponent<oak::TransformComponent>();
	cache_.requireComponent<oak::TextComponent>();
	engine_.getSystem<oak::EntityManager>().addCache(&cache_);
}

void TextSystem::update() {
	auto &renderer = engine_.getSystem<oak::graphics::GLRenderer>();
	for (const auto& entity : cache_.entities()) {
		const auto &tc = entity.getComponent<oak::TransformComponent>();
		const auto &tx = entity.getComponent<oak::TextComponent>();
		renderer.addObject(glm::vec2{ tc.position }, tc.position.z + 0.1f, entity.layer(), tc.rotationAngle, tc.scale, &tx);
	}
}

CollisionSystem::CollisionSystem(oak::Engine &engine) : oak::System{ engine, "collision_system" } {}

void CollisionSystem::init() {
	cache_.requireComponent<oak::TransformComponent>();
	cache_.requireComponent<oak::AABB2dComponent>();
	engine_.getSystem<oak::EntityManager>().addCache(&cache_);
}

struct CollisionInfo {
	glm::vec2 position;
	glm::vec2 extent;
};

bool aabb_vs_aabb(const CollisionInfo &a, const CollisionInfo &b, glm::vec2 &normal, float &depth) {
	glm::vec2 n = b.position - a.position;
	glm::vec2 overlap = a.extent + b.extent - glm::abs(n);

	if (overlap.x > 0 && overlap.y > 0) {
		if (overlap.x < overlap.y) {
			if (n.x < 0) {
				normal = glm::vec2{ -1, 0 };
			} else {
				normal = glm::vec2{ 1, 0 };
			}
			depth = overlap.x;
		} else {
			if (n.y < 0) {
				normal = glm::vec2{ 0, -1 };
			} else {
				normal = glm::vec2{ 0, 1 };
			}
			depth = overlap.y;
		}
		return true;
	}
	return false;
}

void CollisionSystem::update() {
	//check for collisions
	glm::vec2 normal;
	float depth;

	auto &entities = cache_.entities();
	auto &ts = engine_.getSystem<TileSystem>();
	for (size_t i = 0; i < entities.size(); i++) {
		const auto& entityA = entities[i];
		const auto& tcA = entityA.getComponent<oak::TransformComponent>();
		const auto& boxA = entityA.getComponent<oak::AABB2dComponent>();
		//check for tile collisions
		const glm::vec2 tp = glm::floor(glm::vec2{ tcA.position } / 16.0f) * 16.0f + glm::vec2{ 8.0f };
		for (int ix = -8; ix < 8; ix++) {
			for (int iy = -8; iy < 8; iy++) {				
				const Chunk& c = ts.getChunk(tcA.position.x + ix * 16.0f, tcA.position.y + iy * 16.0f, entityA.layer());
				const Tile& t = c.getTile( static_cast<int>(glm::floor(tcA.position.x / 16.0f) + ix) % 16, static_cast<int>(glm::floor(tcA.position.y / 16.0f) + iy) % 16 );
				if ((t.flags & Tile::SOLID) == Tile::SOLID && aabb_vs_aabb(
					{ glm::vec2{ tcA.position } + boxA.offset, boxA.halfExtent }, 
					{ tp + glm::vec2{ ix * 16.0f, iy * 16.0f }, glm::vec2{ 8.0f } },
					normal, depth)) {
					engine_.getEventManager().emitEvent(TileCollisionEvent{ entityA, t, normal, depth });
				}
			}
		}

		//start one entity after entity A so we dont get duplicate pairs or entities colliding with theirselves 
		for (size_t j = i + 1; j < entities.size(); j++) {
			const auto& entityB = entities[j];
			const auto& tcB = entityB.getComponent<oak::TransformComponent>();
			const auto& boxB = entityB.getComponent<oak::AABB2dComponent>();
			if (aabb_vs_aabb(
				{ glm::vec2{ tcA.position } + boxA.offset, boxA.halfExtent }, 
				{ glm::vec2{ tcB.position } + boxB.offset, boxB.halfExtent }, 
				normal, depth)) {
				engine_.getEventManager().emitEvent(oak::EntityCollisionEvent{ entityA, entityB, normal, depth });
				engine_.getEventManager().emitEvent(oak::EntityCollisionEvent{ entityB, entityA, -normal, depth });
			}
		}
	}
}

PhysicsSystem::PhysicsSystem(oak::Engine &engine) : System{ engine, "physics_system" } {}

void PhysicsSystem::init() {
	cache_.requireComponent<oak::TransformComponent>();
	cache_.requireComponent<oak::PhysicsBody2dComponent>();
	engine_.getSystem<oak::EntityManager>().addCache(&cache_);
}

void PhysicsSystem::update(float dt) {

}