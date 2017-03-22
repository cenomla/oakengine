#include "save_manager.h"

#include "file_manager.h"
#include "engine.h"

namespace oak {

	SaveManager::SaveManager(Engine& engine) : System{ engine, "save_manager" }, buffer_{ 512 }, file_{ nullptr }, fsize_{ 0 }, flags_{ 0 } {}


	void SaveManager::open(const oak::string& path, uint32_t flags) {
		if (file_ != nullptr) { return; }
		const char* str;
		if ((flags & SAVE) == SAVE) {
			str = "wb";
		} else if ((flags & LOAD) == LOAD) {
			str = "rb";
		}
		file_ = fopen(Engine::inst().getSystem<FileManager>().resolve(path).c_str(), str);
		if (file_ == nullptr) { //file failed to open file
			std::cout << "failed to open file: " << path << std::endl;
			return;
		}
		//find file size
		fseek(file_, 0, SEEK_END);
		fsize_ = ftell(file_);
		fseek(file_, 0, SEEK_SET);
		flags_ = flags;

		//if we are loading the file
		if ((flags_ & LOAD) == LOAD) {
			buffer_.resize(fsize_);
			fread(buffer_.data(), 1, fsize_, file_);
		}
	}

	void SaveManager::close() {
		if (file_ != nullptr) {
			//write buffer contents to file
			if ((flags_ & SAVE) == SAVE) {
				fwrite(buffer_.data(), 1, buffer_.pos(), file_);
			}
			buffer_.rewind();
			fclose(file_);
			file_ = nullptr;
			flags_ = 0;
		}
	}

}