#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>

namespace oak {
	class Entity;
}

namespace oak::util {

	struct ObjInfo {
		std::string name;

		inline const ObjInfo& combine(const ObjInfo &other) {
			if (!other.name.empty()) {
				name = other.name + "." + name;
			}
			return *this;
		}
	};

	inline const ObjInfo& operator+(ObjInfo &&lhs, const ObjInfo &rhs) {
		return lhs.combine(rhs);
	}

	enum class PuperIo {
		IN,
		OUT
	};

	class Puper {
	public:
		virtual ~Puper();

		virtual void pup(int8_t &data, const ObjInfo &info) = 0;
		virtual void pup(int16_t &data, const ObjInfo &info) = 0;
		virtual void pup(int32_t &data, const ObjInfo &info) = 0;
		virtual void pup(int64_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint8_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint16_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint32_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint64_t &data, const ObjInfo &info) = 0;
		virtual void pup(float &data, const ObjInfo &info) = 0;
		virtual void pup(double &data, const ObjInfo &info) = 0;
		virtual void pup(bool &data, const ObjInfo &info) = 0;
		virtual void pup(Entity &data, const ObjInfo &info) = 0;
	
		inline void setIo(PuperIo io) { io_ = io; }
		inline PuperIo getIo() const { return io_; }
	protected:
		PuperIo io_ = PuperIo::OUT;
	};

	class ByteBuffer;

	class ByteBufferPuper : public Puper {
	public:
		ByteBufferPuper(ByteBuffer &buffer);

		void pup(int8_t &data, const ObjInfo &info) override;
		void pup(int16_t &data, const ObjInfo &info) override;
		void pup(int32_t &data, const ObjInfo &info) override;
		void pup(int64_t &data, const ObjInfo &info) override;
		void pup(uint8_t &data, const ObjInfo &info) override;
		void pup(uint16_t &data, const ObjInfo &info) override;
		void pup(uint32_t &data, const ObjInfo &info) override;
		void pup(uint64_t &data, const ObjInfo &info) override;
		void pup(float &data, const ObjInfo &info) override;
		void pup(double &data, const ObjInfo &info) override;
		void pup(bool &data, const ObjInfo &info) override;
		void pup(Entity &data, const ObjInfo &info) override;

	private:
		ByteBuffer *buffer_;
	};
	
	void pup(Puper &puper, int8_t &data, const ObjInfo &info);
	void pup(Puper &puper, int16_t &data, const ObjInfo &info);
	void pup(Puper &puper, int32_t &data, const ObjInfo &info);
	void pup(Puper &puper, int64_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint8_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint16_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint32_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint64_t &data, const ObjInfo &info);
	void pup(Puper &puper, float &data, const ObjInfo &info);
	void pup(Puper &puper, double &data, const ObjInfo &info);
	void pup(Puper &puper, bool &data, const ObjInfo &info);
	void pup(Puper &puper, glm::vec2 &data, const ObjInfo &info);
	void pup(Puper &puper, glm::vec3 &data, const ObjInfo &info);
	void pup(Puper &puper, glm::vec4 &data, const ObjInfo &info);
	void pup(Puper &puper, Entity &data, const ObjInfo &info);

}