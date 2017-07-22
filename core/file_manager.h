#pragma once

#include <cstdio>

#include "oak_assert.h"
#include "util/byte_buffer.h"
#include "container.h"

namespace oak {

	struct VirtualDirectory {
		oak::string name;
		VirtualDirectory *parent = nullptr;
		oak::vector<VirtualDirectory> children;
		oak::vector<oak::string> links;
	};

	class FileManager {
	private:
		static FileManager *instance;
	public:
		static FileManager& inst() { 
			oak_assert(instance != nullptr);
			return *instance; 
		}

		FileManager();
		~FileManager();

		void mount(const oak::string& path, const oak::string& mountPoint);

		oak::string resolvePath(const oak::string& path);

		ByteBuffer&& readFile(const oak::string& path);
		void writeFile(const oak::string& path, const ByteBuffer& buffer);
	private:
		VirtualDirectory root_;
	};

}