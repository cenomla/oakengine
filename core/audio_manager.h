#pragma once

#include <atomic>

#include "oak_assert.h"
#include "container.h"

struct SoundIo;
struct SoundIoDevice;
struct SoundIoOutStream;

namespace oak {

	namespace detail{ struct AudioUserData; }

	struct AudioSampler {
		enum Flag: uint32_t {
			LOOP = 0x01,
			PAUSED = 0x02
		};
		detail::AudioUserData *data = nullptr;
		size_t length = 0;

		void destroy();

		bool operator==(const AudioSampler& other) const {
			return data == other.data;
		}
	};

	class AudioManager {
		friend void write_callback(SoundIoOutStream*, int, int);
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
		SoundIoOutStream *stream_ = nullptr;

		size_t channelCount_;
		oak::vector<AudioSampler> samplers_;

		void connect();
		void disconnect();

		void createStream();
		void destroyStream();
	};

}