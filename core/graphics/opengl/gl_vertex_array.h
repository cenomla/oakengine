#pragma once

#include <initializer_list>
#include <glad/glad.h>

namespace oak::graphics {

	class GLVertexArray {
	public:
		struct AttributeData {
			uint32_t location;
			uint32_t format;
			size_t offset;
		};

		GLVertexArray();
		~GLVertexArray();

		void create();
		void destroy();
	
		void bind() const;
		void unbind() const;

		void attributeDescription(size_t stride, std::initializer_list<AttributeData> &&attribs);
	private:
		GLuint vao_;
	};

}