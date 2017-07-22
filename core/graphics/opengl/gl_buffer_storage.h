#pragma once

#include "graphics/buffer_storage.h"
#include "graphics/attribute_layout.h"
#include "graphics/buffer.h"

namespace oak::graphics {

	class GLBufferStorage : public BufferStorage {
	public:

		void create(const AttributeLayout *layout) override;
		void destroy() override;

		void instance(const AttributeLayout *layout, size_t offset) override;

		void bind() const override;
		void unbind() const override;

		void* map(int index) override;
		void unmap(int index) override;

		void data(int index, size_t size, const void *data) override;

	private:
		uint32_t vao_;
		Buffer vbo_;
		Buffer ibo_;
	};

}