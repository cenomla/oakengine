#pragma once

#include <cstddef>
#include <cinttypes>

#include <glm/glm.hpp>

#include "container.h"

namespace oak::graphics {

	enum class AttributeType {
		POSITION,
		NORMAL,
		UV,
		COLOR
	};

	struct AttributeLayout {
		oak::vector<AttributeType> attributes;

		size_t stride() const {
			size_t s = 0;
			for (const auto& type : attributes) {
				switch (type) {
					case AttributeType::UV:
						s += 2 * sizeof(float);
						break;
					case AttributeType::COLOR:
						s += 3;
						break;
					default:
						s += 3 * sizeof(float);
				}
			}
			return s;
		}
	};

	struct MeshVertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uvs;
	};

	class Mesh {
	public:
		Mesh(const AttributeLayout& layout);

		void load(const oak::string& path);

		void draw(void *buffer, void *ibuffer, const glm::mat4& transform, uint32_t startIndex) const;

		inline size_t getVertexCount() const { return vertices_.size(); }
		inline size_t getIndexCount() const { return indices_.size(); }
		inline const AttributeLayout& getLayout() const { return layout_; }

	private:
		AttributeLayout layout_;
		oak::vector<MeshVertex> vertices_;
		oak::vector<uint32_t> indices_;
	};

}