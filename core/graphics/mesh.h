#pragma once

#include <cstddef>
#include <cinttypes>

#include <glm/glm.hpp>

#include "container.h"

namespace oak::graphics {

	class Mesh {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
		};
		
		void setData(const oak::vector<Vertex>& vertices, const oak::vector<uint32_t>& indices);

		void draw(void *buffer, void *ibuffer, const glm::mat4& transform, uint32_t startIndex) const;

		inline size_t getVertexCount() const { return vertices_.size(); }
		inline size_t getIndexCount() const { return indices_.size(); }

	private:

		oak::vector<Vertex> vertices_;
		oak::vector<uint32_t> indices_;

		void destroy();
	};

}