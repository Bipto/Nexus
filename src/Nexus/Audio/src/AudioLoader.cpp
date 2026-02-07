#include "Audio/AudioLoader.hpp"
#include "Audio/AudioTypes.hpp"

#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

namespace Nexus::Audio
{
	Audio::AudioFormat GetAudioFormatFFmpeg(int channelCount)
	{
		if (channelCount > 1)
			return Audio::AudioFormat::StereoFloat32;
		else
			return Audio::AudioFormat::MonoFloat32;
	}

	tl::expected<std::shared_ptr<AudioBuffer>, std::string> LoadAudioFileToBufferFFmpeg(const std::string &filepath, AudioDevice *device)
	{
		if (!device)
			return tl::unexpected("Audio device was invalid");

		auto buffer = device->CreateAudioBuffer();
		if (!buffer)
			return tl::unexpected("Failed to create buffer");

		AVFormatContext *formatCtx = nullptr;
		if (avformat_open_input(&formatCtx, filepath.c_str(), nullptr, nullptr) < 0)
			return tl::unexpected("Failed to open audio file");

		if (avformat_find_stream_info(formatCtx, nullptr) < 0)
			return tl::unexpected("Failed to read stream info");

		int audioStreamIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

		if (audioStreamIndex < 0)
			return tl::unexpected("No audio stream found");

		AVStream		  *stream	   = formatCtx->streams[audioStreamIndex];
		AVCodecParameters *codecParams = stream->codecpar;

		const AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);
		if (!codec)
			return tl::unexpected("Unsupported audio codec");

		AVCodecContext *codecCtx = avcodec_alloc_context3(codec);
		if (!codecCtx)
			return tl::unexpected("Failed to allocate codec context");

		if (avcodec_parameters_to_context(codecCtx, codecParams) < 0)
			return tl::unexpected("Failed to copy codec parameters");

		if (avcodec_open2(codecCtx, codec, nullptr) < 0)
			return tl::unexpected("Failed to open codec");

		// -----------------------------
		// Resampler (to interleaved float)
		// -----------------------------
		SwrContext *swr = nullptr;

		AVChannelLayout outLayout;
		av_channel_layout_default(&outLayout, codecCtx->ch_layout.nb_channels);

		if (swr_alloc_set_opts2(&swr,
								&outLayout,
								AV_SAMPLE_FMT_FLT,
								codecCtx->sample_rate,
								&codecCtx->ch_layout,
								codecCtx->sample_fmt,
								codecCtx->sample_rate,
								0,
								nullptr) < 0)
		{
			return tl::unexpected("Failed to create resampler");
		}

		if (swr_init(swr) < 0)
			return tl::unexpected("Failed to initialize resampler");

		std::vector<float> samples;

		AVPacket *packet = av_packet_alloc();
		AVFrame	 *frame	 = av_frame_alloc();

		if (!packet || !frame)
			return tl::unexpected("Failed to allocate packet/frame");

		// -----------------------------
		// Decode loop
		// -----------------------------
		while (av_read_frame(formatCtx, packet) >= 0)
		{
			if (packet->stream_index == audioStreamIndex)
			{
				if (avcodec_send_packet(codecCtx, packet) >= 0)
				{
					while (avcodec_receive_frame(codecCtx, frame) == 0)
					{
						int outSamples = av_rescale_rnd(swr_get_delay(swr, codecCtx->sample_rate) + frame->nb_samples,
														codecCtx->sample_rate,
														codecCtx->sample_rate,
														AV_ROUND_UP);

						uint8_t *outData	 = nullptr;
						int		 outLineSize = 0;

						av_samples_alloc(&outData, &outLineSize, outLayout.nb_channels, outSamples, AV_SAMPLE_FMT_FLT, 0);

						int converted = swr_convert(swr, &outData, outSamples, (const uint8_t **)frame->data, frame->nb_samples);

						float *floatData = reinterpret_cast<float *>(outData);

						samples.insert(samples.end(), floatData, floatData + converted * outLayout.nb_channels);

						av_freep(&outData);
					}
				}
			}
			av_packet_unref(packet);
		}

		int sampleRate = codecCtx->sample_rate;

		// -----------------------------
		// Cleanup
		// -----------------------------
		av_frame_free(&frame);
		av_packet_free(&packet);
		swr_free(&swr);
		avcodec_free_context(&codecCtx);
		avformat_close_input(&formatCtx);

		if (samples.empty())
			return tl::unexpected("Decoded audio was empty");

		// -----------------------------
		// Fill AudioBuffer
		// -----------------------------
		size_t fileSize = samples.size() * sizeof(float);

		Audio::AudioFormat format = GetAudioFormatFFmpeg(outLayout.nb_channels);

		buffer->SetData(samples.data(), fileSize, format, sampleRate);

		return buffer;
	}

	tl::expected<std::shared_ptr<AudioBuffer>, std::string> AudioLoader::LoadAudioFile(const std::string &filepath, AudioDevice *device)
	{
		return LoadAudioFileToBufferFFmpeg(filepath, device);
	}
}	 // namespace Nexus::Audio