#pragma once

#include <cstdio>

#include "oak_assert.h"
#include "util/stream.h"
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

		oak::string resolvePath(const oak::string& path, bool canCreate = false);

		Stream openFile(const oak::string& path, bool canCreate = false);
		void closeFile(Stream& stream);
	private:
		VirtualDirectory root_;
	};

}