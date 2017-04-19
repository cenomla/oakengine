#pragma once

#include "container.h"

namespace oak {

	class FileManager {
	public:
		void mount(const oak::string& path, const oak::string& mountPoint);
		oak::string resolve(const oak::string& path);
	private:
		oak::unordered_map<oak::string, oak::string> mounts_;
	};

}