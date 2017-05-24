#pragma once

#include <glm/glm.hpp>

#include "graphics/font.h"
#include "graphics/renderable.h"
#include "graphics/sprite.h"
#include "graphics/mesh.h"
#include "container.h"
#include "resource.h"

namespace oak {

	class Puper;
	struct ObjInfo;

	struct TransformComponent {
		glm::mat4 transform{ 1.0f };
	};

	struct MeshComponent {
		Resource<graphics::Mesh> mesh{ 0 };
	};

	struct PositionComponent {
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

		SpriteComponent() = default;
		SpriteComponent(size_t id, const glm::vec2& s, int afx, int afy, uint32_t l) : sprite{ id }, scale{ s }, animFrameX{ afx }, animFrameY{ afy }, layer{ l } {}

		Resource<graphics::Sprite> sprite{ 0 };
		glm::vec2 scale{ 1.0f };
		int animFrameX = 0, animFrameY = 0;
		uint32_t layer = 0;

		void draw(void *buffer, float x, float y, float rotation, float s) const override { 
			sprite.get().draw(buffer, x, y, animFrameX, animFrameY, rotation, scale.x * s, scale.y * s);
		}

		size_t getMaterialId() const override { return sprite.get().getMaterialId(); }
		size_t getVertexCount() const override { return 4; }
	};

	struct TextComponent : public graphics::Renderable {
		Resource<graphics::Font> font{ 0 };
		oak::string text;
		glm::vec2 offset{ 0.0f };
		uint32_t layer = 0;
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

	void pup(Puper& puper, TransformComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, MeshComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, PositionComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, AABB2dComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, SpriteComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, TextComponent& comp, const ObjInfo& info);
	void pup(Puper& puper, PhysicsBody2dComponent& comp, const ObjInfo& info);

}