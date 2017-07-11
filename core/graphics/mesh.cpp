#include "mesh.h"

#include "util/file_util.h"
#include "util/string_util.h"

namespace oak::graphics {

	void Mesh::setData(const oak::vector<Vertex>& vertices, const oak::vector<uint32_t>& indices) {
		vertices_ = vertices;
		indices_ = indices;
	}

	void Mesh::draw(void *buffer, void *ibuffer, const glm::mat4& transform, uint32_t startIndex) const {

		for (const auto& vertex : vertices_) {
			*static_cast<glm::vec3*>(buffer) = glm::vec3{ transform * glm::vec4{ vertex.position, 1.0f } };
			buffer = static_cast<glm::vec3*>(buffer) + 1;
			*static_cast<glm::vec3*>(buffer) = glm::vec3{ transform * glm::vec4{ vertex.normal, 0.0f } };
			buffer = static_cast<glm::vec3*>(buffer) + 1;
			*static_cast<glm::vec2*>(buffer) = vertex.uv;
			buffer = static_cast<glm::vec2*>(buffer) + 1;
		}

		int i = 0;
		for (auto index : indices_) {
			static_cast<uint32_t*>(ibuffer)[i++] = index + startIndex;
		}
	}

}