#pragma once

#include <string>
#include <bitset>
#include <glm/glm.hpp>

#include "config.h"
#include "util/puper.h"
#include "prefab.h"
#include "resource.h"

namespace graphics { class Sprite; }

namespace oak {

	struct PrefabComponent {
		Resource<Prefab> prefab;
		std::bitset<config::MAX_COMPONENTS> ownsMask;
	};

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
		Resource<graphics::Sprite> sprite;
		int animFrameX;
		int animFrameY;
	};

	struct PhysicsBody2dComponent {
		PhysicsBody2dComponent(const glm::vec2 &iv, const glm::vec2 &f, float m, float r, float sf, float df) : 
			velocity{ iv }, 
			force{ f }, 
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

	struct CallbackComponent {
		std::function<void (Entity e)> callback;
	};

	namespace util {
		void pup(Puper &puper, PrefabComponent &comp, const ObjInfo &info);
		void pup(Puper &puper, TransformComponent &comp, const ObjInfo &info);
		void pup(Puper &puper, AABB2dComponent &comp, const ObjInfo &info);
		void pup(Puper &puper, SpriteComponent &comp, const ObjInfo &info);
		void pup(Puper &puper, PhysicsBody2dComponent &comp, const ObjInfo &info);
	}

}