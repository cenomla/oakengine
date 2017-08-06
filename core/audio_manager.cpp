#include "audio_manager.h"

#include <algorithm>
#include <cmath>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <soundio/soundio.h>
#include <glm/glm.hpp>

#include "file_manager.h"
#include "oak_alloc.h"
#include "log.h"

namespace oak {

	namespace detail {
		struct AudioUserData {
			stb_vorbis *vorbis = nullptr;
			stb_vorbis_info info;

			float *buffer = nullptr;
			size_t pos = 0;

			std::atomic<uint32_t> flags{ 0 };
			std::atomic<float> volume{ 1.0f };
		};
	}

	void write_callback(SoundIoOutStream *outStream, int frameCountMin, int frameCountMax) {
		AudioManager *ud = static_cast<AudioManager*>(outStream->userdata);
		if (!ud) { return; }
		const SoundIoChannelLayout *layout = &outStream->layout;
		SoundIoChannelArea *areas;
		int framesLeft = std::clamp(frameCountMin, 4410, frameCountMax);
		int err;
		float sample;
		float samples[2];
		//soundio_outstream_clear_buffer(outStream);


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
				sample = 0.0f;
				for (auto& sampler : ud->samplers_) {
					if (sampler.data->flags & AudioSampler::PAUSED) {
						continue;
					}
					if (sampler.data->buffer) {
						sample += sampler.data->buffer[sampler.data->pos++];
					} else if (sampler.data->vorbis) {
						stb_vorbis_get_samples_float_interleaved(sampler.data->vorbis, ud->channelCount_, samples, 2);
						sampler.data->pos++;
						sample += samples[0];
					}
					if (sampler.data->pos >= sampler.length) {
						sampler.data->pos = 0;
						if (sampler.data->vorbis) {
							stb_vorbis_seek_start(sampler.data->vorbis);
						}
						sampler.data->flags |= AudioSampler::PAUSED;
					}
				}

				for (int c = 0; c < layout->channel_count; c++) {
					*reinterpret_cast<float*>(areas[c].ptr) = sample; 
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

	void AudioSampler::destroy() {
		AudioManager::inst().destroySound(*this);
	}

	AudioManager *AudioManager::instance = nullptr;

	AudioManager::AudioManager() : channelCount_{ 2 } {
		oak_assert(!instance);
		instance = this;
		connect();
		createStream();
	}

	AudioManager::~AudioManager() {
		oak_assert(instance);
		instance = nullptr;
		destroyStream();
		disconnect();
	}

	void AudioManager::playSound(AudioSampler& sampler, bool play) {
		sampler.data->flags ^= ((play ? 0 : 0xffffffff) ^ sampler.data->flags) & AudioSampler::PAUSED; //set paused flag to play 
	}

	AudioSampler AudioManager::createSound(const oak::string& path, uint32_t flags) {
		auto resolvedPath = FileManager::inst().resolvePath(path);
		//allocate sound data
		detail::AudioUserData *ud = static_cast<detail::AudioUserData*>(oalloc_freelist.allocate(sizeof(detail::AudioUserData)));
		new (ud) detail::AudioUserData{};

		ud->vorbis = stb_vorbis_open_filename(resolvedPath.c_str(), nullptr, nullptr);
		if (!ud->vorbis) {
			log_print_err("failed to open audio file: %s", path.c_str());
			abort();
		}

		ud->info = stb_vorbis_get_info(ud->vorbis);
		ud->flags = flags;

		size_t length = stb_vorbis_stream_length_in_samples(ud->vorbis);

		if (length < 1000000) {
			ud->buffer = static_cast<float*>(oalloc_freelist.allocate(length * sizeof(float)));
			stb_vorbis_get_samples_float_interleaved(ud->vorbis, 1, ud->buffer, length);
		}

		samplers_.push_back({ ud, length });
		return samplers_.back();
	}

	void AudioManager::destroySound(AudioSampler& sampler) {
		samplers_.erase(std::remove(std::begin(samplers_), std::end(samplers_), sampler), std::end(samplers_));

		if (sampler.data->buffer) {
			oalloc_freelist.deallocate(sampler.data->buffer, sampler.length * sizeof(float));
			sampler.data->buffer = nullptr;
		}

		if (sampler.data->vorbis) {
			stb_vorbis_close(sampler.data->vorbis);
			sampler.data->vorbis = nullptr;
		}

		sampler.data->detail::AudioUserData::~AudioUserData();
		oalloc_freelist.deallocate(sampler.data, sizeof(detail::AudioUserData));
		
		sampler.data = nullptr;
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

	void AudioManager::createStream() {
		int err = SoundIoErrorNone;
		stream_ = soundio_outstream_create(device_);
		if (!stream_) {
			log_print_err("soundio out of memory");
			abort();
		}
		stream_->format = SoundIoFormatFloat32NE;
		stream_->write_callback = write_callback;
		stream_->userdata = this;
		// stream_->software_latency = 0.1;

		err = soundio_outstream_open(stream_);
		if (err) {
			log_print_err("soundio error: %s", soundio_strerror(err));
			abort();
		}

		if (stream_->layout_error) {
			log_print_err("layout error: %s", soundio_strerror(stream_->layout_error));
			abort();
		}

		err = soundio_outstream_start(stream_);
		if (err) {
			log_print_err("soundio err: %s", soundio_strerror(err));
			abort();
		}


		log_print_out("audio latency: %f", stream_->software_latency);
	}

	void AudioManager::destroyStream() {
		soundio_outstream_destroy(stream_);
		stream_ = nullptr;
	}

}