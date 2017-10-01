#pragma once

#include <cinttypes>

#include "math.h"
#include "container.h"
#include "resource.h"

namespace oak::graphics {

	struct Mesh {
		static const TypeInfo typeInfo;

		struct Vertex {
			Vec3 position;
			Vec3 normal;
			Vec2 uv;
		};

		oak::vector<Vertex> vertices;
		oak::vector<uint32_t> indices;
	};

	oak::vector<Mesh> loadModel(const oak::string& path);

	void pup(Puper& puper, Mesh& data, const ObjInfo& info);

}
