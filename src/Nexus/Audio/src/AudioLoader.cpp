#include "Audio/AudioLoader.hpp"
#include "Audio/AudioTypes.hpp"

#include "libnyquist/Common.h"
#include "libnyquist/Decoders.h"

namespace Nexus::Audio
{
	Audio::AudioFormat GetAudioFormat(nqr::PCMFormat format, int channelCount)
	{
		bool stereo = channelCount > 1;
		switch (format)
		{
			case nqr::PCMFormat::PCM_U8:
			case nqr::PCMFormat::PCM_S8:
			{
				if (stereo)
				{
					return Audio::AudioFormat::Stereo8;
				}
				else
				{
					return Audio::AudioFormat::Mono8;
				}
			}
			case nqr::PCMFormat::PCM_16:
			case nqr::PCMFormat::PCM_24:
			{
				if (stereo)
				{
					return Audio::AudioFormat::Stereo16;
				}
				else
				{
					return Audio::AudioFormat::Mono16;
				}
			}
			case nqr::PCMFormat::PCM_32:
			case nqr::PCMFormat::PCM_64:
			case nqr::PCMFormat::PCM_FLT:
			case nqr::PCMFormat::PCM_DBL:
			{
				if (stereo)
				{
					return Audio::AudioFormat::StereoFloat32;
				}
				else
				{
					return Audio::AudioFormat::MonoFloat32;
				}
			}
			case nqr::PCMFormat::PCM_END: throw std::runtime_error("Invalid audio format");
			default: throw std::runtime_error("Failed to find a valid audio format");
		}
	}

	std::expected<std::shared_ptr<AudioBuffer>, std::string> LoadAudioFileToBuffer(const std::string &filepath,
																				   AudioDevice		 *device,
																				   nqr::BaseDecoder	 *decoder)
	{
		if (!device)
		{
			return std::unexpected("Audio device was invalid");
		}

		std::shared_ptr<AudioBuffer> buffer = device->CreateAudioBuffer();

		if (!buffer)
		{
			return std::unexpected("Failed to create buffer");
		}

		nqr::AudioData data;
		decoder->LoadFromPath(&data, filepath);

		if (data.samples.empty())
		{
			return std::unexpected("Failed to load audio data from file: " + filepath);
		}

		int				   bitsPerSample = nqr::GetFormatBitsPerSample(data.sourceFormat);
		size_t			   fileSize		 = data.samples.size() * sizeof(float);
		int				   sampleRate	 = data.sampleRate;
		Audio::AudioFormat format		 = GetAudioFormat(data.sourceFormat, data.channelCount);
		const void *const  dataPtr		 = data.samples.data();

		buffer->SetData(dataPtr, fileSize, format, sampleRate);

		return buffer;
	}

	std::expected<std::shared_ptr<AudioBuffer>, std::string> AudioLoader::LoadWavFile(const std::string &filepath, AudioDevice *device)
	{
		nqr::WavDecoder decoder;
		return LoadAudioFileToBuffer(filepath, device, &decoder);
	}

	std::expected<std::shared_ptr<AudioBuffer>, std::string> AudioLoader::LoadMp3File(const std::string &filepath, AudioDevice *device)
	{
		nqr::Mp3Decoder decoder;
		return LoadAudioFileToBuffer(filepath, device, &decoder);
	}
}	 // namespace Nexus::Audio