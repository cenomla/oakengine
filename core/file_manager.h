#pragma once

#include "container.h"
#include "system.h"

namespace oak {

	class FileManager : public System {
	public:
		FileManager(Engine &engine);

		void mount(const oak::string& path, const oak::string& mountPoint);
		oak::string resolve(const oak::string& path);
	private:
		oak::unordered_map<oak::string, oak::string> mounts_;
	};

}