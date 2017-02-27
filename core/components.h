#pragma once

#include <string>
#include <bitset>
#include <glm/glm.hpp>

#include "config.h"
#include "graphics/sprite.h"
#include "graphics/font.h"
#include "graphics/renderable.h"
#include "prefab.h"
#include "resource.h"
#include "entity.h"
#include "pup.h"

namespace oak {

	struct PrefabComponent {
		Resource<Prefab> prefab;
		std::bitset<config::MAX_COMPONENTS> ownsMask;
	};

	struct TransformComponent {
		glm::vec3 position{ 0.0f };
		float scale = 1.0f;
		glm::vec3 rotationAxis{ 0.0f };
		float rotationAngle = 0.0f;
	};

	struct AABB2dComponent {
		glm::vec2 halfExtent{ 0.0f };
		glm::vec2 offset{ 0.0f };
	};

	struct SpriteComponent : public graphics::Renderable {
		Resource<graphics::Sprite> sprite{ 0 };
		int animFrameX = 0;
		int animFrameY = 0;

		void draw(void *buffer, float x, float y, float rotation, float scale) const override { 
			sprite.get().draw(buffer, x, y, animFrameX, animFrameY, rotation, scale); 
		}
		size_t getMaterialId() const override { return sprite.get().getMaterialId(); }
		size_t getVertexCount() const override { return 4; }
	};

	struct TextComponent : public graphics::Renderable {
		Resource<graphics::Font> font{ 0 };
		oak::string text{ "" };
		glm::vec2 offset{ 0.0f };
		float size = 24.0f;

		void draw(void *buffer, float x, float y, float rotation, float scale) const override { 
			font.get().draw(buffer, text, x + offset.x, y + offset.y, rotation, size * scale);
		}
		size_t getMaterialId() const override { return font.get().getMaterialId(); }
		size_t getVertexCount() const override { return text.size() * 4; } 
	};

	struct PhysicsBody2dComponent {
		PhysicsBody2dComponent() = default;
		PhysicsBody2dComponent(const glm::vec2 &iv, const glm::vec2 &f, float m, float r, float sf, float df) : 
			velocity{ iv }, 
			force{ f }, 
			mass{ m },
			invMass{ m == 0.0f ? 0.0f : 1.0f / m },
			restitution{ r },
			staticFriction{ sf },
			dynamicFriction{ df } {}

		glm::vec2 velocity{ 0.0f };
		glm::vec2 force{ 0.0f };
		float mass = 0.0f;
		float invMass = 0.0f;
		float restitution = 0.0f;
		float staticFriction = 0.0f;
		float dynamicFriction = 0.0f;
	};

	struct CallbackComponent {
		std::function<void (Entity e)> callback;
	};

	void pup(Puper &puper, PrefabComponent &comp, const ObjInfo &info);
	void pup(Puper &puper, TransformComponent &comp, const ObjInfo &info);
	void pup(Puper &puper, AABB2dComponent &comp, const ObjInfo &info);
	void pup(Puper &puper, SpriteComponent &comp, const ObjInfo &info);
	void pup(Puper &puper, TextComponent &comp, const ObjInfo &info);
	void pup(Puper &puper, PhysicsBody2dComponent &comp, const ObjInfo &info);

}