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

		void attributeDescription(const AttributeLayout& attribs);
	private:
		uint32_t vao_;
	};

}