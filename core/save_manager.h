#pragma once

#include <cstdio>

#include "system.h"
#include "util/byte_buffer.h"

namespace oak {

	class SaveManager : public System {
	public:
		static constexpr uint32_t SAVE = 0x01;
		static constexpr uint32_t LOAD = 0x02;

		SaveManager(Engine& engine);

		void open(const oak::string& path, uint32_t flags);
		void close();

		inline util::ByteBuffer* getBuffer() { return &buffer_; }
		inline uint32_t flags() const { return flags_; }
		inline bool isOpen() const { return file_ != nullptr; }

	private:
		util::ByteBuffer buffer_;
		FILE *file_;
		size_t fsize_;
		uint32_t flags_;
	};
}