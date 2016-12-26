#pragma once

#include <glm/glm.hpp>
#include <entity.h>

#include "chunk.h"

struct TileCollisionEvent {
	oak::Entity entity;
	Tile tile;
	glm::vec2 normal;
	float depth;
};

namespace oak::util {

	class Puper;
	class ObjInfo;

	void pup(Puper &puper, TileCollisionEvent &data, const ObjInfo &info);

}