#include "file_manager.h"

#include "log.h"

namespace oak {

	FileManager::FileManager(Engine &engine) : System{ engine, "file_manager" } {}

	void FileManager::mount(const oak::string& path, const oak::string& mountPoint) {
		mounts_.insert({ path, mountPoint });
	}

	oak::string FileManager::resolve(const oak::string& path) {
		const auto it = mounts_.find(path);
		if (it != std::end(mounts_)) {
			return it->second;
		} else {
			log::cout << "failed to resolve file path: " << path << std::endl;
			abort();
		}
	}

}