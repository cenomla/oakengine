#pragma once

#include "oak_assert.h"
#include "container.h"

struct SoundIo;
struct SoundIoDevice;
struct SoundIoOutStream;

namespace oak {

	class AudioManager {
	private:
		static AudioManager *instance;
	public:
		inline static AudioManager& inst() { 
			oak_assert(instance);
			return *instance;
		}

		AudioManager();
		~AudioManager();

		void update();

		void play();
		void load(const oak::string& path);		

	private:
		SoundIo *soundio_ = nullptr;
		SoundIoDevice *device_ = nullptr;
		SoundIoOutStream *stream_ = nullptr;

		void connect();
		void disconnect();
	};

}