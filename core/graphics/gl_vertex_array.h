#pragma once

#include <cinttypes>
#include <cstddef>

namespace oak::graphics {
	struct AttributeLayout;
}

namespace oak::graphics::GLVertexArray {

	uint32_t create();
	void destroy(uint32_t id);
	
	void bind(uint32_t id);
	void unbind();

	void attribDescription(const AttributeLayout& attribs);
	void instanceAttribDescription(const AttributeLayout& attribs, size_t offset);

}