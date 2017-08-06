#include "audio_manager.h"

#include <algorithm>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <soundio/soundio.h>
#include <cmath>

#include "file_manager.h"
#include "oak_alloc.h"
#include "log.h"

namespace oak {

	namespace detail {
		struct AudioUserData {
			stb_vorbis *vorbis = nullptr;
			stb_vorbis_info info;
		};
	}

	void write_callback(SoundIoOutStream *outStream, int frameCountMin, int frameCountMax) {
		AudioManager *ud = static_cast<AudioManager*>(outStream->userdata);
		if (!ud) { return; }
		const SoundIoChannelLayout *layout = &outStream->layout;
		SoundIoChannelArea *areas;
		int framesLeft = frameCountMax;
		int err;		

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
				if (ud->bufferReadPos_ >= ud->bufferLength_) {
					ud->bufferReadPos_ = 0;
				}

				for (int c = 0; c < layout->channel_count; c++) {
					*reinterpret_cast<float*>(areas[c].ptr) = ud->audioBuffer_[ud->bufferReadPos_]; 
					areas[c].ptr += areas[c].step;
				}

				ud->bufferReadPos_++;
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
		bufferLength_ = 96000 * channelCount_;
		audioBuffer_ = static_cast<float*>(oalloc_freelist.allocate(bufferLength_ * 4));
		connect();
		createStreams();
	}

	AudioManager::~AudioManager() {
		oak_assert(instance);
		instance = nullptr;
		destroyStreams();
		disconnect();
		oalloc_freelist.deallocate(audioBuffer_, bufferLength_ * 4);
	}

	void AudioManager::playSound(AudioSampler& sampler, bool play) {
		sampler.flags ^= ((play ? 0xffffffff : 0) ^ sampler.flags) & AudioSampler::PAUSED; //set paused flag to play 
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

		size_t length = stb_vorbis_stream_length_in_samples(ud->vorbis);

		samplers_.push_back({ ud, length, length, flags });
		return samplers_.back();;
	}

	void AudioManager::destroySound(AudioSampler& sampler) {
		samplers_.erase(std::remove(std::begin(samplers_), std::end(samplers_), sampler), std::end(samplers_));

		stb_vorbis_close(sampler.data->vorbis);
		sampler.data->vorbis = nullptr;

		sampler.data->detail::AudioUserData::~AudioUserData();
		oalloc_freelist.deallocate(sampler.data, sizeof(detail::AudioUserData));
		
		sampler.data = nullptr;
	}

	void AudioManager::update() {
		soundio_flush_events(soundio_);

		float s0, s1, s2, s3, s4, s5, s6, s7;
		float *sbuffer[8]{ &s0, &s1, &s2, &s3, &s4, &s5, &s6, &s7 };

		const size_t maxWrites = 2048;
		size_t writes = 0;

		//fill up ring buffer
		while (bufferWritePos_ != bufferReadPos_) {
			float totalSample[8]{ 0.0f };

			//mix samplers
			for (auto& sampler : samplers_) {
				if (sampler.pos == 0 || samplers.flags & AudioSampler::PAUSED) {
					continue;
				}
				int count = stb_vorbis_get_samples_float(sampler.data->vorbis, channelCount_, sbuffer, 1);
				if (!count) {
					continue;
				}

				for (int i = 0; i < 8; i++) {
					totalSample[i] += *sbuffer[i] * sampler.volume;
				}

				sampler.pos--;
				if (sampler.pos == 0) {
					stb_vorbis_seek_start(sampler.data->vorbis);
					sampler.pos = sampler.length;
					if (!sampler.flags & AudioSampler::LOOP) {
						//pause clip
						sampler.flags &= ~AudioSampler::PAUSED;
					}
				}
			}

			audioBuffer_[bufferWritePos_++] = totalSample[0];
			if (bufferWritePos_ >= bufferLength_) {
				bufferWritePos_ = 0;
			}
			writes++;
		}


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

	void AudioManager::createStreams() {
		int err = SoundIoErrorNone;
		SoundIoOutStream *stream = soundio_outstream_create(device_);
		if (!stream) {
			log_print_err("soundio out of memory");
			abort();
		}
		stream->format = SoundIoFormatFloat32NE;
		stream->write_callback = write_callback;
		stream->userdata = this;

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

		streams_.push_back(stream);
	}

	void AudioManager::destroyStreams() {
		for (auto& stream : streams_) {
			soundio_outstream_destroy(stream);
			stream = nullptr;
		}
	}

}