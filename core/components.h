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

	struct AABB2DComponent {
		glm::vec2 center;
		glm::vec2 halfExtents;
	};

	struct SpriteComponent {
		const graphics::Sprite *sprite;
		int animFrameX;
		int animFrameY;
	};

	struct KinematicBody2DComponent {
		glm::vec2 velocity;

	};

}