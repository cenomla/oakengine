#include "file_manager.h"

#include <experimental/filesystem>

#include "log.h"


namespace oak {
namespace fs = std::experimental::filesystem;

	FileManager::FileManager(Engine &engine) : System{ engine, "file_manager" } {}

	void FileManager::mount(const oak::string& path, const oak::string& mountPoint) {
		mounts_.insert({ mountPoint, path });
	}

	oak::string FileManager::resolve(const oak::string& path) {
		size_t pos = path.find("::");
		if (pos != oak::string::npos) {
			oak::string str = path.substr(0, pos);
			const auto it = mounts_.find(str);
			if (it != std::end(mounts_)) {
				return it->second + path.substr(pos+2);
			} else {
				log::cout << "failed to resolve file path: " << path << std::endl;
				abort();
			}
		} else {
			return path;
		}
	}

}