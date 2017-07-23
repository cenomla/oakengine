#include "file_buffer.h"

namespace oak {

	FileBuffer::FileBuffer(FILE *file) : file_{ file }, owns_{ false } {

	}

	FileBuffer::FileBuffer(const char *path) : owns_{ true } {
		file_ = fopen(path, "r+b");
	}

	FileBuffer::~FileBuffer() {
		if (owns_ && file_) {
			fclose(file_);
			file_ = nullptr;
		}
	}

	void FileBuffer::set() {
		mark_ = ftell(file_);		
	}

	void FileBuffer::reset() {
		fseek(file_, mark_, SEEK_SET);
		mark_ = 0;
	}

	void FileBuffer::rewind() {
		fseek(file_, 0, SEEK_SET);
	}

	size_t FileBuffer::read(size_t size, void *data) {
		return fread(data, 1, size, file_);
	}

	size_t FileBuffer::write(size_t size, const void *data) {
		return fwrite(data, 1, size, file_);
	}

}