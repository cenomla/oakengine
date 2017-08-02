#include "file_manager.h"

#include <experimental/filesystem>

#include "util/file_buffer.h"
#include "util/string_util.h"
#include "log.h"

namespace oak {
namespace fs = std::experimental::filesystem;

	FileManager *FileManager::instance = nullptr;

	FileManager::FileManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	FileManager::~FileManager() {
		instance = nullptr;
	}

	void FileManager::mount(const oak::string& path, const oak::string& mountPoint) {
		//resolve path on real filesystem (replace {$variables})
		oak::string resolvedPath;
		const oak::string home = getenv("HOME");
		const oak::string install = home + "/Workspace/OakEngine";
		const oak::string cwd = ".";
		if (path.front() == '{') {
			size_t pos = path.find('}');
			const oak::string &t = path.substr(0, pos + 1);
			const oak::string suffix = (pos + 2 >= path.size()) ? "" : path.substr(pos + 1);
			if (t == "{$cwd}") {
				resolvedPath = cwd + suffix;
			} else if (t == "{$installDir}") {
				resolvedPath = install + suffix;
			} else if (t == "{$userHome}") {
				resolvedPath = home + suffix;
			} else {
				resolvedPath = path;
			}
		}

		oak::string delimeters{ "/" };
		oak::vector<oak::string> tokens;

		util::splitstr(mountPoint, delimeters, tokens);

		VirtualDirectory *dir = &root_;

		//find virtual fs dir node child that matches the current token then proceed into that directory until the end of tokens are reached
		for (const auto& token : tokens) {
			if (token.empty()) { continue; }
			for (auto& child : dir->children) {//travel down the directory tree
				if (child.name == token) {
					dir = &child;
					goto found;
				}
			}
			//no virtual directory found so create one
			dir->children.push_back({ token, dir });
			dir = &dir->children.back();
			found:
			continue;
		}
		//once we have found the appropriate virtual fs node then we add the path to its links vector
		dir->links.push_back(resolvedPath);
	}

	oak::string FileManager::resolvePath(const oak::string& path, bool canCreate) {
		oak::string delimeters{ "/" };
		oak::vector<oak::string> tokens;

		util::splitstr(path, delimeters, tokens);

		VirtualDirectory *dir = &root_;

		auto it = std::begin(tokens);
		auto end = std::end(tokens);
		for (; it != end; ++it) {
			for (auto& child : dir->children) {
				if (child.name == *it) {
					dir = &child;
					goto found;
				}
			}
			//reached end of virtual fs tree
			break;
			found:
			continue;
		}

		//we now have the virtual directory so try to resolve the path with all of its links

		//first reconstruct the path namst reconstruct the path name
		oak::string pathSuffix;

		for (; it != end; ++it) {
			pathSuffix += "/" + *it;
		}

		//append path suffix to end of the virtual directories links and check if the resulting path is a valid file

		for (const auto& link : dir->links) {
			oak::string fullPath = link + pathSuffix;
			if (fs::exists(fullPath)) {
				return fullPath;
			}
		}

		if (canCreate && !dir->links.empty()) {
			return dir->links[0] + pathSuffix;
		} else {
			return "";
		}
	}

	Stream FileManager::openFile(const oak::string& path, bool canCreate) {
		const oak::string resolvedPath = resolvePath(path, canCreate);
		FILE *file = fopen(resolvedPath.c_str(), canCreate ? "w+b" : "r+b");

		if (!file) {
			log_print_err("failed to open file: %s", path.c_str());
			abort();
		}

		FileBuffer *buffer = static_cast<FileBuffer*>(oalloc_freelist.allocate(sizeof(FileBuffer)));
		new (buffer) FileBuffer{ file };

		return { buffer };
	}

	void FileManager::closeFile(Stream& stream) {
		stream.buffer->~BufferBase();
		oalloc_freelist.deallocate(stream.buffer, sizeof(FileBuffer));
		stream.buffer = nullptr;
	}

}