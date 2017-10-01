#include "audio_manager.h"

#include <algorithm>
#include <cmath>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <soundio/soundio.h>

#include "file_manager.h"
#include "oak_alloc.h"
#include "log.h"

namespace oak {

	namespace detail {
		struct AudioSampler {
			stb_vorbis *vorbis = nullptr;
			stb_vorbis_info info;

			float *buffer = nullptr;
			size_t length = 0;
		};
	}

	void write_callback(SoundIoOutStream *outStream, int frameCountMin, int frameCountMax) {
		AudioManager *ud = static_cast<AudioManager*>(outStream->userdata);
		if (!ud) { return; }
		const SoundIoChannelLayout *layout = &outStream->layout;
		SoundIoChannelArea *areas;
		// int framesLeft = std::clamp(frameCountMin, 4410, frameCountMax);
		int framesLeft = frameCountMax;
		int err;
		float sample;
		float samples[2];

		//add any new audio samples if nessesary
		if (ud->hasToAdd_.load() && ud->playlist_.size() < AudioManager::MAX_AUDIO_INSTANCES) {
			ud->playlist_.push_back(ud->toAdd_);
			ud->hasToAdd_.store(false);
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
				sample = 0.0f;
				for (auto& instance : ud->playlist_) {
					if (instance.flags & AudioObject::PAUSED) {
						continue;
					}
					if (instance.data->buffer) {
						sample += instance.data->buffer[instance.pos++];
					} else if (instance.data->vorbis) {
						stb_vorbis_get_samples_float_interleaved(instance.data->vorbis, ud->channelCount_, samples, 2);
						instance.pos++;
						sample += samples[0];
					}
					if (instance.pos >= instance.data->length) {
						instance.pos = 0;
						if (instance.data->vorbis) {
							stb_vorbis_seek_start(instance.data->vorbis);
						}
						if (!(instance.flags & AudioObject::LOOP)) {
							instance.flags |= AudioObject::PAUSED;
							instance.flags |= AudioObject::DONE;
						}
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

		ud->playlist_.erase(std::remove_if(std::begin(ud->playlist_), std::end(ud->playlist_), 
			[](const auto& inst){ return inst.flags & AudioObject::DONE; }), std::end(ud->playlist_));
	}

	void AudioObject::destroy() {
		AudioManager::inst().destroySound(*this);
	}

	AudioManager *AudioManager::instance = nullptr;

	AudioManager::AudioManager() : channelCount_{ 2 } {
		oak_assert(!instance);
		instance = this;
		playlist_.reserve(MAX_AUDIO_INSTANCES);
		connect();
		createStream();
	}

	AudioManager::~AudioManager() {
		oak_assert(instance);
		instance = nullptr;
		destroyStream();
		disconnect();
	}

	void AudioManager::playSound(AudioObject& audio, uint32_t flags, float volume) {
		if (audio.id < 0) { return; }
		auto sampler = samplers_[audio.id];
		if (!hasToAdd_.load()) {
			toAdd_ = { sampler, 0, flags, volume };
			hasToAdd_.store(true);
		}
	}

	AudioObject AudioManager::createSound(const oak::string& path) {
		auto resolvedPath = FileManager::inst().resolvePath(path);
		//allocate sound data
		detail::AudioSampler *sampler = static_cast<detail::AudioSampler*>(oalloc_freelist.allocate(sizeof(detail::AudioSampler)));
		new (sampler) detail::AudioSampler{};

		sampler->vorbis = stb_vorbis_open_filename(resolvedPath.c_str(), nullptr, nullptr);
		if (!sampler->vorbis) {
			log_print_err("failed to open audio file: %s", path.c_str());
			abort();
		}

		sampler->info = stb_vorbis_get_info(sampler->vorbis);

		size_t length = stb_vorbis_stream_length_in_samples(sampler->vorbis);

		if (length < 1000000) {
			sampler->buffer = static_cast<float*>(oalloc_freelist.allocate(length * sizeof(float)));
			stb_vorbis_get_samples_float_interleaved(sampler->vorbis, 1, sampler->buffer, length);
			sampler->length = length;
		}

		samplers_.push_back(sampler);
		return { static_cast<int>(samplers_.size() - 1) };
	}

	void AudioManager::destroySound(AudioObject& audio) {
		if (audio.id < 0) { return; }
		auto& sampler = samplers_[audio.id];

		samplers_.erase(std::remove(std::begin(samplers_), std::end(samplers_), sampler), std::end(samplers_));

		if (sampler->buffer) {
			oalloc_freelist.deallocate(sampler->buffer, sampler->length * sizeof(float));
			sampler->buffer = nullptr;
		}

		if (sampler->vorbis) {
			stb_vorbis_close(sampler->vorbis);
			sampler->vorbis = nullptr;
		}

		sampler->detail::AudioSampler::~AudioSampler();
		oalloc_freelist.deallocate(sampler, sizeof(detail::AudioSampler));
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
		stream_->software_latency = 0.1;

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
