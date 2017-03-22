#pragma once

#include <cstdio>

#include "system.h"

namespace oak {

	class SaveManager : public System {
	public:
		static constexpr uint32_t SAVE = 0x01;
		static constexpr uint32_t LOAD = 0x02;

		SaveManager(Engine& engine);

		void open(const oak::string& path, uint32_t flags);
		void close();

		inline FILE* getFile() { return file_; }
		inline size_t getFileSize() { return fsize_; }
		inline uint32_t flags() const { return flags_; }
		inline bool isOpen() const { return file_ != nullptr; }

	private:
		FILE *file_;
		size_t fsize_;
		uint32_t flags_;
	};
}