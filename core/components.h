#pragma once

#include <glm/glm.hpp>

namespace graphics { class Sprite; }

namespace oak {

	struct TransformComponent {
		glm::vec3 position;
		float scale;
		glm::vec3 rotationAxis;
		float rotationAngle;
	};

	struct AABB2dComponent {
		glm::vec2 halfExtent;
		glm::vec2 offset;
	};

	struct SpriteComponent {
		const graphics::Sprite *sprite;
		int animFrameX;
		int animFrameY;
	};

	struct PhysicsBody2dComponent {
		PhysicsBody2dComponent(const glm::vec2 &iv, float m, float r, float sf, float df) : 
			velocity{ iv }, 
			force{ 0.0f }, 
			mass{ m },
			invMass{ m == 0.0f ? 0.0f : 1.0f / m }, 
			restitution{ r },
			staticFriction{ sf },
			dynamicFriction{ df } {}

		glm::vec2 velocity;
		glm::vec2 force;
		float mass;
		float invMass;
		float restitution;
		float staticFriction;
		float dynamicFriction;
	};

}