#pragma once

#include <cstddef>
#include <cinttypes>

#include <glm/glm.hpp>

#include "container.h"

#include "attribute_layout.h"

namespace oak::graphics {

	class Mesh {
	public:
		Mesh(const AttributeLayout *layout);
		~Mesh();

		void setData(void *vertices, void *indices, size_t count, size_t icount);
		void load(const oak::string& path);

		void draw(void *buffer, void *ibuffer, const glm::mat4& transform, uint32_t startIndex) const;

		inline size_t getVertexCount() const { return vertexCount_; }
		inline size_t getIndexCount() const { return indexCount_; }
		inline const AttributeLayout* getLayout() const { return layout_; }

	private:
		const AttributeLayout *layout_;
		size_t vertexCount_, indexCount_;
		void *data_, *idata_;
	};

}