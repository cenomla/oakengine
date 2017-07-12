#pragma once

#include <cinttypes>

namespace oak::graphics {
	struct AttributeLayout;
}

namespace oak::graphics::GLVertexArray {

	uint32_t create();
	void destroy(uint32_t id);
	
	void bind(uint32_t id);
	void unbind();

	void attributeDescription(const AttributeLayout *attribs);

}