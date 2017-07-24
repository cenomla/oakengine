#pragma once

#include <glm/glm.hpp>
#include <mesh2d.h>

namespace oak {
	class Puper;
	struct ObjInfo;
}

struct TransformComponent {
	glm::mat3 transform;
};

struct MeshComponent {
	const oak::Mesh2d *mesh;
};

struct ManifoldComponent {
	bool collides = false;
};

void pup(oak::Puper& puper, TransformComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, MeshComponent& data, const oak::ObjInfo& info);
void pup(oak::Puper& puper, ManifoldComponent& data, const oak::ObjInfo& info);


