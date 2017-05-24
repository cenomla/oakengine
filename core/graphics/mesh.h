#pragma once

#include <cstddef>
#include <cinttypes>

#include <glm/glm.hpp>

#include "container.h"

namespace oak::graphics {

	struct AttributeDescriptor {
		static constexpr uint8_t FLOAT_T = 0;
		static constexpr uint8_t INT_T = 1;
		static constexpr uint8_t UINT_T = 2;
		static constexpr uint8_t BYTE_T = 4;
		static constexpr uint8_t UBYTE_T = 8;

		uint32_t location = 0;
		uint32_t count = 0;
		uint32_t type = 0;
		size_t offset = 0;
	};

	struct MeshVertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uvs;
	};

	struct MeshDescriptor {
		size_t stride = 0;
		oak::vector<AttributeDescriptor> attributes;
	};

	class Mesh {
	public:
		Mesh(const MeshDescriptor& descriptor);

		void load(const oak::string& path);

		void draw(void *buffer, const glm::mat4& transform) const;

		inline size_t getVertexCount() const { return vertices_.size(); }
		inline const MeshDescriptor& getDescriptor() const { return descriptor_; }

	private:
		MeshDescriptor descriptor_;
		oak::vector<MeshVertex> vertices_;
	};

}