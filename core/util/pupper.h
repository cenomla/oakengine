#pragma once

#include <cstdint>

namespace oak::util {

	struct ObjInfo {

	};

	enum class PupperIo {
		IN,
		OUT
	};

	class Pupper {
	public:
		virtual ~Pupper();

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
	
		inline void setIo(PupperIo io) { io_ = io; }
	protected:
		PupperIo io_ = PupperIo::OUT;
	};

	class ByteBuffer;

	class ByteBufferPupper : public Pupper {
	public:
		ByteBufferPupper(ByteBuffer &buffer);

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

	private:
		ByteBuffer *buffer_;
	};
	
	void pup(Pupper &pupper, int8_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, int16_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, int32_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, int64_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, uint8_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, uint16_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, uint32_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, uint64_t &data, const ObjInfo &info);
	void pup(Pupper &pupper, float &data, const ObjInfo &info);
	void pup(Pupper &pupper, double &data, const ObjInfo &info);
	void pup(Pupper &pupper, bool &data, const ObjInfo &info);

}