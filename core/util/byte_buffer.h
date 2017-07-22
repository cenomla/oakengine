#pragma once

#include <cstddef>

#include "oak_alloc.h"
#include "stream.h"

namespace oak {

	class ByteBuffer : public BufferBase {
	public:
		ByteBuffer(size_t size, Allocator *allocator = &oalloc_freelist);
		ByteBuffer(void *data, size_t size);
		~ByteBuffer();
		
		ByteBuffer(const ByteBuffer& other);
		void operator=(const ByteBuffer& oter);

		ByteBuffer(ByteBuffer&& other);
		void operator=(ByteBuffer&& other);

		void read(size_t size, void *data) override;
		void write(size_t size, const void *data) override;

		void set() override;
		void reset() override;
		void rewind() override;
		void resize(size_t nsize);

		inline size_t capacity() const { return capacity_; }
		inline size_t pos() const { return pos_; }

		inline char* data() { return buffer_; }
		inline const char* data() const { return buffer_; }
	private:
		Allocator *allocator_;
		size_t capacity_;
		char *buffer_;
		size_t pos_;
		size_t mark_;
		bool owns_;

		void init();
		void destroy();
		void checkResize(size_t size);
	};

}