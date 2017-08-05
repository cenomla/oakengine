
#include "audio_manager.h"

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <soundio/soundio.h>
#include <cmath>
#include <atomic>

#include "file_manager.h"
#include "oak_alloc.h"
#include "log.h"

namespace oak {

	struct AudioUserData {
		stb_vorbis *vorbis = nullptr;
		stb_vorbis_info info;
		std::atomic<uint32_t> flags{ 0 };
	};

	static void write_callback(SoundIoOutStream *outStream, int frameCountMin, int frameCountMax) {
		AudioUserData *ud = static_cast<AudioUserData*>(outStream->userdata);
		if (!ud) { return; }
		const SoundIoChannelLayout *layout = &outStream->layout;
		SoundIoChannelArea *areas;
		int framesLeft = frameCountMax;
		int err;

		float sample;
		float *buffer[16] { &sample };

		bool paused = ud->flags & AudioSampler::PAUSED;
		if (paused) {
			return;
		}


		while (framesLeft > 0) {
			int frameCount = framesLeft;

			err = soundio_outstream_begin_write(outStream, &areas, &frameCount);
			if (err) {
				log_print_err("soundio error: %s", soundio_strerror(err));
				abort();
			}

			if (!frameCount) {
				break;
			}

			//write to buffer
			for (int f = 0; f < frameCount; f++) {
				int count = stb_vorbis_get_samples_float(ud->vorbis, ud->info.channels, buffer, 1);
				if (!count) {
					if (ud->flags & AudioSampler::LOOP) {
						stb_vorbis_seek_start(ud->vorbis);
						continue;
					}
					ud->flags |= AudioSampler::PAUSED;
					return;
				}
				for (int c = 0; c < layout->channel_count; c++) {
					*reinterpret_cast<float*>(areas[c].ptr) = *buffer[c % ud->info.channels]; 
					areas[c].ptr += areas[c].step;
				}
			}

			err = soundio_outstream_end_write(outStream);
			if (err) {
				log_print_err("soundio error: %s", soundio_strerror(err));
				abort();
			}


			framesLeft -= frameCount;
		}
	}

	AudioManager *AudioManager::instance = nullptr;

	static AudioSampler audioSampler0;
	static AudioSampler audioSampler1;

	AudioManager::AudioManager() {
		oak_assert(!instance);
		instance = this;
		connect();
		audioSampler0 = createSound("/res/test.ogg", AudioSampler::LOOP);
		audioSampler1 = createSound("/res/chip.ogg", AudioSampler::LOOP);
	}

	AudioManager::~AudioManager() {
		oak_assert(instance);
		instance = nullptr;
		destroySound(audioSampler0);
		destroySound(audioSampler1);
		disconnect();
	}

	void AudioManager::playSound(AudioSampler& sampler) {
		auto ud = static_cast<AudioUserData*>(sampler.stream->userdata);
		ud->flags &= (~AudioSampler::PAUSED); //unset paused flag
		//soundio_outstream_pause(sampler.stream, ud->flags & AudioSampler::PAUSED);
	}

	AudioSampler AudioManager::createSound(const oak::string& path, uint32_t flags) {
		auto resolvedPath = FileManager::inst().resolvePath(path);
		int err;
		AudioSampler sampler;
		//allocate sound data
		AudioUserData *ud = static_cast<AudioUserData*>(oalloc_freelist.allocate(sizeof(AudioUserData)));
		new (ud) AudioUserData{};

		ud->vorbis = stb_vorbis_open_filename(resolvedPath.c_str(), nullptr, nullptr);
		if (!ud->vorbis) {
			log_print_err("failed to open audio file: %s", path.c_str());
			abort();
		}

		ud->info = stb_vorbis_get_info(ud->vorbis);
		ud->flags = flags;

		SoundIoOutStream *stream;
		stream = soundio_outstream_create(device_);
		if (!stream) {
			log_print_err("soundio out of memory");
			abort();
		}
		stream->format = SoundIoFormatFloat32NE;		
		stream->write_callback = write_callback;
		stream->userdata = ud;

		err = soundio_outstream_open(stream);
		if (err) {
			log_print_err("soundio error: %s", soundio_strerror(err));
			abort();
		}

		if (stream->layout_error) {
			log_print_err("layout error: %s", soundio_strerror(stream->layout_error));
			abort();
		}

		err = soundio_outstream_start(stream);
		if (err) {
			log_print_err("soundio err: %s", soundio_strerror(err));
			abort();
		}

		return { stream };
	}

	void AudioManager::destroySound(AudioSampler& sampler) {
		auto ud = static_cast<AudioUserData*>(sampler.stream->userdata);
		sampler.stream->userdata = nullptr;
		//destroy stream
		soundio_outstream_destroy(sampler.stream);
		sampler.stream = nullptr;
		//destroy the vorbis stream
		stb_vorbis_close(ud->vorbis);
		ud->vorbis = nullptr;
		//deallocate sampler user data
		ud->~AudioUserData();
		oalloc_freelist.deallocate(ud, sizeof(AudioUserData));
	}

	void AudioManager::update() {
		soundio_flush_events(soundio_);
	}

	void AudioManager::connect() {
		int err;
		soundio_ = soundio_create();
		if (!soundio_) {
			log_print_err("soundio out of memory");

			abort();
		}

		err = soundio_connect(soundio_);
		if (err) {
			log_print_err("soundio error: %s", soundio_strerror(err));
			abort();
		}

		soundio_flush_events(soundio_);

		int defaultOutDeviceIndex = soundio_default_output_device_index(soundio_);
		if (defaultOutDeviceIndex < 0) {
			log_print_err("no output device found");
			abort();
		}

		device_ = soundio_get_output_device(soundio_, defaultOutDeviceIndex);
		if (!device_) {
			log_print_err("soundio out of memory");
			abort();
		}

		log_print_out("output device: %s", device_->name);
	}

	void AudioManager::disconnect() {
		soundio_device_unref(device_);
		soundio_destroy(soundio_);
	}
	

}