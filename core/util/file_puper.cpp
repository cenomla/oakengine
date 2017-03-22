#include "file_puper.h"

#include "util/byte_buffer.h"
#include "entity.h"

namespace oak {

	FilePuper::FilePuper(FILE *file) : file_{ file } {}

	void FilePuper::pup(int8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(int16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(int32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(int64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(uint8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(uint16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(uint32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(uint64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(float& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(double& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(bool& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(&data, sizeof(data), 1, file_);
		} else {
			fread(&data, sizeof(data), 1, file_);
		}
	}

	void FilePuper::pup(oak::string& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			fwrite(data.c_str(), 1, data.size(), file_);
		} else {
			char str[1024];
			size_t i = 0;
			char c = '\0';
			do {
				c = static_cast<char>(fgetc(file_));
				str[i] = c;
				i++;
			} while (c != 0);
			data = { str, i };
		}
	}

	void FilePuper::pup(Entity& data, const ObjInfo& info) {
		uint64_t id = data.id();
		uintptr_t m = reinterpret_cast<uintptr_t>(data.getManager());
		if (io_ == PuperIo::OUT) {
			fwrite(&id, sizeof(id), 1, file_);
			fwrite(&m, sizeof(m), 1, file_);
		} else {
			fread(&id, sizeof(id), 1, file_);
			fread(&m, sizeof(m), 1, file_);
			data = { id, reinterpret_cast<EntityManager*>(m) };
		}
	}

}