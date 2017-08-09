#pragma once

#include <atomic>
#include <mutex>

#include "oak_assert.h"
#include "container.h"

struct SoundIo;
struct SoundIoDevice;
struct SoundIoOutStream;

namespace oak {

	namespace detail{ 
		struct AudioSampler; 

		struct AudioInstance {
			AudioSampler *data = nullptr;
			size_t pos = 0;
			uint32_t flags = 0;
			float volume = 1.0f;
		};
	}

	struct AudioObject {
		enum Flag: uint32_t {
			LOOP = 0x01,
			PAUSED = 0x02,
			DONE = 0x04
		};
		int id = -1;

		void destroy();
	};

	class AudioManager {
		friend void write_callback(SoundIoOutStream*, int, int);
	private:
		static AudioManager *instance;
	public:
		static constexpr size_t MAX_AUDIO_INSTANCES = 128;

		inline static AudioManager& inst() {
			oak_assert(instance);
			return *instance;
		}

		AudioManager();
		~AudioManager();

		void update();

		void playSound(AudioObject& audio, uint32_t flags, float volume);
		AudioObject createSound(const oak::string& path);		
		void destroySound(AudioObject& audio);

	private:
		SoundIo *soundio_ = nullptr;
		SoundIoDevice *device_ = nullptr;
		SoundIoOutStream *stream_ = nullptr;

		size_t channelCount_;
		oak::vector<detail::AudioSampler*> samplers_;
		oak::vector<detail::AudioInstance> playlist_;
		detail::AudioInstance toAdd_;
		std::atomic<bool> hasToAdd_;

		void connect();
		void disconnect();

		void createStream();
		void destroyStream();
	};

}