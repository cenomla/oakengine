#pragma once

#include "graphics/buffer_storage.h"
#include "graphics/attribute_layout.h"

#include "gl_buffer.h"
#include "gl_vertex_array.h"

namespace oak::graphics {

	class GLBufferStorage : public BufferStorage {
	public:

		GLBufferStorage();

		void create(const AttributeLayout *attribs) override;
		void destroy() override;

		void bind() const override;
		void unbind() const override;

		void* map(int index, uint32_t flags) override;
		void unmap(int index) override;

		void data(int index, size_t size, const void *data, uint32_t flags) override;

	private:
		GLVertexArray vao_;
		GLBuffer vbo_;
		GLBuffer ibo_;
	};

}