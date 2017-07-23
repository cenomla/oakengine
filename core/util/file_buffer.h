#pragma once

#include <cstdio>

#include "stream.h"

namespace oak {

	class FileBuffer : public BufferBase {
	public:
		FileBuffer(FILE *file);
		FileBuffer(const char *path);
		~FileBuffer();

		void set() override;
		void reset() override;
		void rewind() override;

		size_t read(size_t size, void *data) override;
		size_t write(size_t size, const void *data) override;

		size_t size() const override;

	private:
		FILE *file_ = nullptr;
		int mark_ = 0;
		bool owns_;
	};

}