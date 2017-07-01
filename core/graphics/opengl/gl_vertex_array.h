#pragma once

#include "graphics/mesh.h"

namespace oak::graphics {

	class GLVertexArray {
	public:

		GLVertexArray();
		~GLVertexArray();

		void create();
		void destroy();
	
		void bind() const;
		void unbind() const;

		void attributeDescription(const AttributeLayout *attribs) const;
	private:
		uint32_t vao_;
	};

}