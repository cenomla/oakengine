
#include "audio_manager.h"

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <soundio/soundio.h>
#include <cmath>

#include "file_manager.h"
#include "log.h"

namespace oak {

	struct AudioUserData {
		stb_vorbis *vorbis;
		stb_vorbis_info info;
	};

	static AudioUserData data;

	static void write_callback(SoundIoOutStream *outStream, int frameCountMin, int frameCountMax) {
		AudioUserData *ud = static_cast<AudioUserData*>(outStream->userdata);
		const SoundIoChannelLayout *layout = &outStream->layout;
		SoundIoChannelArea *areas;
		int framesLeft = frameCountMax;
		int err;

		float sample;
		float *buffer[16] { &sample };

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
					//stb_vorbis_seek_start(ud->vorbis);
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

	AudioManager::AudioManager() {
		oak_assert(!instance);
		instance = this;
		load("/res/test.ogg");
		connect();
	}

	AudioManager::~AudioManager() {
		oak_assert(instance);
		instance = nullptr;
		disconnect();
		if (data.vorbis) {
			stb_vorbis_close(data.vorbis);
		}
	}

	void AudioManager::load(const oak::string& path) {
		auto resolvedPath = FileManager::inst().resolvePath(path);
		data.vorbis = stb_vorbis_open_filename(resolvedPath.c_str(), nullptr, nullptr);
		if (!data.vorbis) {
			log_print_err("failed to open audio file: %s", path.c_str());
			abort();
		}

		data.info = stb_vorbis_get_info(data.vorbis);
		

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

		stream_ = soundio_outstream_create(device_);
		if (!stream_) {
			log_print_err("soundio out of memory");
			abort();
		}
		stream_->format = SoundIoFormatFloat32NE;		
		stream_->write_callback = write_callback;
		stream_->userdata = &data;

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
	}

	void AudioManager::disconnect() {
		soundio_outstream_destroy(stream_);
		soundio_device_unref(device_);
		soundio_destroy(soundio_);
	}
	

}