#include "save_manager.h"

SaveManager::SaveManager(oak::Engine& engine) : System{ engine, "save_manager" }, buffer_{ 512 }, file_{ nullptr }, fsize_{ 0 }, flags_{ 0 } {}


void SaveManager::open(const oak::string& path, uint32_t flags) {
	if (file_ != nullptr) { return; }
	const char* str;
	if ((flags & SAVE) == SAVE) {
		str = "wb";
	} else if ((flags & LOAD) == LOAD) {
		str = "rb";
	}
	file_ = fopen(path.c_str(), str);
	if (file_ == nullptr) { //file failed to open file
		std::cout << "failed to open file: " << path << std::endl;
		abort();
	}
	//find file size
	fseek(file_, 0, SEEK_END);
	fsize_ = ftell(file_);
	fseek(file_, 0, SEEK_SET);
	flags_ = flags;
}

void SaveManager::close() {
	fclose(file_);
	file_ = nullptr;
	flags_ = 0;
}