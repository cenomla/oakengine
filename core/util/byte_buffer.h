#pragma once

#include <cstddef>

#include "array.h"
#include "oak_alloc.h"
#include "stream.h"

namespace oak {

	class ByteBuffer : public BufferBase {
	public:
		void init(size_t size);
		void destroy();

		size_t read(size_t size, void *data) override;
		size_t write(size_t size, const void *data) override;

		void set() override;
		void reset() override;
		void rewind() override;
		void resize(size_t nsize);

		inline size_t size() const override { return storage_.capacity; }
		inline size_t pos() const { return pos_; }

		inline char* data() { return storage_.data; }
		inline const char* data() const { return storage_.data; }
	private:
		Array<char> storage_;
		size_t pos_;
		size_t mark_;

		void checkResize(size_t size);
	};

}
