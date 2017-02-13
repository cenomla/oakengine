#pragma once

#include <initializer_list>
#include <glad/glad.h>

#include "graphics/attribute_data.h"

namespace oak::graphics {

	class GLVertexArray {
	public:
		GLVertexArray();
		~GLVertexArray();

		void create();
		void destroy();
	
		void bind() const;
		void unbind() const;

		void attributeDescription(BindingData &&binding, std::initializer_list<AttributeData> &&attribs);
	private:
		GLuint vao_;
	};

}