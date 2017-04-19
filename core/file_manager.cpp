#include "file_manager.h"

#include <experimental/filesystem>

#include "log.h"

namespace oak {
namespace fs = std::experimental::filesystem;

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
				log_print_err("failed to resolve file path: %s", path.c_str());
				abort();
			}
		} else {
			return path;
		}
	}

}