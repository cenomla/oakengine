#pragma once

#include "oak_assert.h"
#include "container.h"

struct SoundIo;
struct SoundIoDevice;
struct SoundIoOutStream;

namespace oak {

	struct AudioSampler {
		enum Flag: uint32_t {
			LOOP = 0x01,
			PAUSED = 0x02
		};
		SoundIoOutStream *stream = nullptr;

		void destroy();
	};

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

		void playSound(AudioSampler& sampler, bool play);
		AudioSampler createSound(const oak::string& path, uint32_t flags);		
		void destroySound(AudioSampler& sampler);

	private:
		SoundIo *soundio_ = nullptr;
		SoundIoDevice *device_ = nullptr;

		void connect();
		void disconnect();
	};

}