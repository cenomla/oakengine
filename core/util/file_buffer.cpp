#include "file_buffer.h"

namespace oak {

	FileBuffer::FileBuffer(FILE *file) : file_{ file } {

	}

	FileBuffer::~FileBuffer() {
		if (file_) {
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

	size_t FileBuffer::size() const {
		size_t pos = ftell(file_);
		fseek(file_, 0, SEEK_END);
		size_t size = ftell(file_);
		fseek(file_, pos, SEEK_SET);
		return size;
	}

}