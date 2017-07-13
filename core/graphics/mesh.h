#pragma once

#include <cstddef>
#include <cinttypes>
#include <glm/glm.hpp>

#include "texture.h"
#include "container.h"

namespace oak::graphics {

	struct Mesh {
		oak::vector<float> data;
		oak::vector<uint32_t> indices;
		size_t vertexCount = 0;
	};

	template<class T>
	inline void meshSetTextureRegion(Mesh *mesh, const TextureRegion& region) {
		glm::vec2 scale{ region.dw, region.dh };
		glm::vec2 pos{ region.dx, region.dy };
		T *v = reinterpret_cast<T*>(mesh->data.data());
		for (size_t i = 0; i < mesh->vertexCount; i++) {
			v->uv *= scale;
			v->uv += pos;
			v++;
		}
	}

}