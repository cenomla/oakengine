#include "save_manager.h"

#include "file_manager.h"
#include "log.h"

namespace oak {

	void SaveManager::open(const oak::string& path, uint32_t flags) {
		if (file_ != nullptr) { return; }
		const char* str = "";
		if ((flags & SAVE) == SAVE) {
			str = "wb";
		} else if ((flags & LOAD) == LOAD) {
			str = "rb";
		}
		file_ = fopen(path.c_str(), str);
		if (file_ == nullptr) { //file failed to open file
			log_print_out("failed to open file: %s", path.c_str());
			return;
		}
		//find file size
		fseek(file_, 0, SEEK_END);
		fsize_ = ftell(file_);
		fseek(file_, 0, SEEK_SET);
		flags_ = flags;
	}

	void SaveManager::close() {
		if (file_ != nullptr) {
			fclose(file_);
			file_ = nullptr;
			flags_ = 0;
		}
	}

}