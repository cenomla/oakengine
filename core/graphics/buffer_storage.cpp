#include "buffer_storage.h"

#include "graphics/opengl/gl_api.h"

namespace oak::graphics {

	void BufferStorage::create(const AttributeLayout& layout) {
		//standard opengl vertex array setup with index buffer
		vao_ = vertex_array::create();
		vertex_array::bind(vao_);

		BufferInfo info;
		info.type = BufferType::VERTEX;
		info.hint = BufferHint::STREAM;
		buffers_[0] = buffer::create(info);
		buffer::bind(buffers_[0]);
		vertex_array::attribDescription(layout);

		info.type = BufferType::ELEMENT;
		buffers_[1] = buffer::create(info);
		buffer::bind(buffers_[1]);
		
		vertex_array::unbind();
	}

	void BufferStorage::instance(const AttributeLayout& layout, size_t offset) {
		buffer::bind(buffers_[0]);
		vertex_array::instanceAttribDescription(layout, offset);
	}

	void BufferStorage::destroy() {
		//destroy opengl objects
		buffer::destroy(buffers_[0]);
		buffer::destroy(buffers_[1]);
		vertex_array::destroy(vao_);
	}

	void BufferStorage::bind() const {
		vertex_array::bind(vao_);
	}

	void BufferStorage::unbind() const {
		vertex_array::unbind();
	}

	void* BufferStorage::map(int index) {
		if (index == 0) {
			buffer::bind(buffers_[0]);
			return buffer::map(buffers_[0]);
		}
		if (index == 1) {
			buffer::bind(buffers_[1]);
			return buffer::map(buffers_[1]);
		}
		return nullptr;
	}

	void BufferStorage::unmap(int index) {
		if (index == 0) {
			buffer::unmap(buffers_[0]);
		}
		if (index == 1) {
			buffer::unmap(buffers_[1]);
		}
	}

	void BufferStorage::data(int index, size_t size, const void *data) {
		if (index == 0) {
			buffer::bind(buffers_[0]);
			buffer::data(buffers_[0], size, data);
		}
		if (index == 1) {
			buffer::bind(buffers_[1]);
			buffer::data(buffers_[1], size, data);
		}
	}

}