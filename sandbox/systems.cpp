#include "systems.h"

#include <glm/glm.hpp>
#include <system_manager.h>
#include <graphics/opengl/gl_renderer.h>
#include <component_storage.h>
#include <components.h>

#include "tile_system.h"

SpriteSystem::SpriteSystem(oak::Scene& scene) : scene_{ &scene } {}

void SpriteSystem::init() {
	cache_.requireComponent<oak::TransformComponent>();
	cache_.requireComponent<oak::SpriteComponent>();
}

void SpriteSystem::run() {
	cache_.update(*scene_);
	oak::ComponentStorage& ts = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, oak::TransformComponent>::id);
	oak::ComponentStorage& ss = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, oak::SpriteComponent>::id);
	auto &renderer = oak::SystemManager::inst().getSystem<oak::graphics::GLRenderer>();
	for (const auto& entity : cache_.entities()) {
		const auto& tc = *static_cast<const oak::TransformComponent*>(ts.getComponent(entity));
		const auto& sc = *static_cast<const oak::SpriteComponent*>(ss.getComponent(entity));
		renderer.addObject(glm::vec2{ tc.position }, tc.position.z, sc.layer, tc.rotationAngle, tc.scale, &sc);
	}
}

TextSystem::TextSystem(oak::Scene& scene) : scene_{ &scene } {}

void TextSystem::init() {
	cache_.requireComponent<oak::TransformComponent>();
	cache_.requireComponent<oak::TextComponent>();
}

void TextSystem::run() {
	cache_.update(*scene_);
	oak::ComponentStorage& ts = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, oak::TransformComponent>::id);
	oak::ComponentStorage& xs = scene_->getComponentStorage(oak::util::type_id<oak::detail::BaseComponent, oak::TextComponent>::id);
	auto &renderer = oak::SystemManager::inst().getSystem<oak::graphics::GLRenderer>();
	for (const auto& entity : cache_.entities()) {
		const auto& tc = *static_cast<const oak::TransformComponent*>(ts.getComponent(entity));
		const auto& tx = *static_cast<const oak::TextComponent*>(xs.getComponent(entity));
		renderer.addObject(glm::vec2{ tc.position }, tc.position.z + 0.1f, tx.layer, tc.rotationAngle, tc.scale, &tx);
	}
}

struct CollisionInfo {
	glm::vec2 position;
	glm::vec2 extent;
};

bool aabb_vs_aabb(const CollisionInfo& a, const CollisionInfo& b, glm::vec2& normal, float& depth) {
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
