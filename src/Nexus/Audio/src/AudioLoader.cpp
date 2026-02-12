#include "Audio/AudioLoader.hpp"
#include "Audio/AudioTypes.hpp"

#include <filesystem>
#include <vector>

#include <sndfile.h>

namespace Nexus::Audio
{
	tl::expected<std::shared_ptr<AudioBuffer>, std::string> LoadAudioFileToBuffer(const std::string &filepath, AudioDevice *device)
	{
		if (!device)
			return tl::unexpected("Audio device was invalid");

		std::shared_ptr<AudioBuffer> buffer = device->CreateAudioBuffer();
		if (!buffer)
			return tl::unexpected("Failed to create buffer");

		SF_INFO	 sfInfo {};
		SNDFILE *sndFile = sf_open(filepath.c_str(), SFM_READ, &sfInfo);

		if (!sndFile)
			return tl::unexpected("Failed to open audio file: " + filepath);

		// libsndfile gives you PCM samples; choose float for consistency
		std::vector<float> samples(sfInfo.frames * sfInfo.channels);

		sf_count_t readCount = sf_readf_float(sndFile, samples.data(), sfInfo.frames);
		sf_close(sndFile);

		if (readCount == 0)
			return tl::unexpected("Failed to read audio samples: " + filepath);

		// Determine OpenAL format
		Audio::AudioFormat format;
		if (sfInfo.channels == 1)
			format = Audio::AudioFormat::MonoFloat32;
		else if (sfInfo.channels == 2)
			format = Audio::AudioFormat::StereoFloat32;
		else
			return tl::unexpected("Unsupported channel count");

		const void *dataPtr	 = samples.data();
		size_t		dataSize = samples.size() * sizeof(float);

		buffer->SetData(dataPtr, dataSize, format, sfInfo.samplerate);

		return buffer;
	}

	tl::expected<std::shared_ptr<AudioBuffer>, std::string> AudioLoader::LoadAudioFile(const std::string &filepath, AudioDevice *device)
	{
		return LoadAudioFileToBuffer(filepath, device);
	}
}	 // namespace Nexus::Audio