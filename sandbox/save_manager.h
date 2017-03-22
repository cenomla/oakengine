#pragma once

#include <cstdio>
#include <system.h>
#include <util/byte_buffer.h>
#include <util/byte_buffer_puper.h>

class SaveManager : public oak::System {
public:
	static constexpr uint32_t SAVE = 0x01;
	static constexpr uint32_t LOAD = 0x02;

	SaveManager(oak::Engine& engine);

	void open(const oak::string& path, uint32_t flags);
	void close();
	
	template<class T>
	void stream(T& data, const oak::string& name) {
		//make sure we are saving/loading
		if (!((flags_ & SAVE) == SAVE || (flags_ & LOAD) == LOAD)) {
			return;
		}
		oak::ByteBufferPuper puper{ buffer_ };
		if ((flags_ & LOAD) == LOAD) {
			puper.setIo(oak::PuperIo::IN);
		}
		//if we are loading the file
		if ((flags_ & LOAD) == LOAD) {
			buffer_.resize(fsize_);
			fread(buffer_.data(), 1, fsize_, file_);
		}
		//pup the object
		pup(puper, data, oak::ObjInfo{ name });
		//write buffer contents to file
		if ((flags_ & SAVE) == SAVE) {
			fwrite(buffer_.data(), 1, buffer_.pos(), file_);
		}
		
		//resert the buffers position 
		buffer_.rewind();
	}
	
	inline bool isOpen() const { return file_ != nullptr; }

private:
	oak::util::ByteBuffer buffer_;
	FILE *file_;
	size_t fsize_;
	uint32_t flags_;
};