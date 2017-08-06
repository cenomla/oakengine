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
		size_t length = 0, pos = 0;
		uint32_t flags = 0;
		float volume = 1.0f;

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

		size_t channelCount_;

		float *audioBuffer_ = nullptr;
		size_t bufferLength_ = 0;
		std::atomic<size_t> bufferReadPos_{ 0 };
		std::atomic<size_t> bufferWritePos_{ 0 };

		oak::vector<SoundIoOutStream*> streams_;
		oak::vector<AudioSampler> samplers_;


		void connect();
		void disconnect();

		void createStreams();
		void destroyStreams();
	};

}