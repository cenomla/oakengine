#pragma once

#include <glm/glm.hpp>
#include <collision.h>

namespace oak {
	class Puper;
	struct ObjInfo;
}

struct TransformComponent {
	glm::mat3 transform;
};

struct VelocityComponent {
	glm::vec2 velocity;
	glm::vec2 acceleration;
};

struct MeshComponent {
	const oak::Mesh2d *mesh;
};

struct RigidBodyComponent {
	float restituion = 0.2f;
	float mass = 0.0f;
	float invMass = mass == 0.0f ? 0.0f : 1.0f / mass;
};

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, VelocityComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, MeshComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, RigidBodyComponent& data, const oak::ObjInfo& info);


