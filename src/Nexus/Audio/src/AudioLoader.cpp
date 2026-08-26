#include "Audio/AudioLoader.hpp"
#include "Audio/AudioTypes.hpp"

#include <expected>
#include <filesystem>
#include <unordered_set>
#include <vector>

#include <sndfile.h>

namespace Nexus::Audio
{
    std::expected<AudioBufferHandle, std::string> LoadAudioFileToBuffer(const std::string &filepath,
                                                                        AudioDevice *device)
    {
        if (!device)
            return std::unexpected("Audio device was invalid");

        AudioBufferHandle buffer = device->CreateAudioBuffer();
        if (!buffer.IsValid())
            return std::unexpected("Failed to create buffer");

        SF_INFO info{};
        SNDFILE *sndFile = sf_open(filepath.c_str(), SFM_READ, &info);

        if (!sndFile)
            return std::unexpected("Failed to open audio file: " + filepath);

        // Read samples as float32
        std::vector<float> samples(info.frames * info.channels);
        sf_count_t framesRead = sf_readf_float(sndFile, samples.data(), info.frames);
        sf_close(sndFile);

        if (framesRead == 0)
            return std::unexpected("Failed to read audio samples: " + filepath);

        // Determine audio format
        Audio::AudioFormat format;
        if (info.channels > 1)
            format = Audio::AudioFormat::StereoFloat32;
        else
            format = Audio::AudioFormat::MonoFloat32;

        buffer->SetData(samples.data(), samples.size() * sizeof(float), format, info.samplerate);

        return buffer;
    }

    std::expected<AudioBufferHandle, std::string> AudioLoader::LoadAudioFile(const std::string &filepath,
                                                                             AudioDevice *device)
    {
        std::filesystem::path path = filepath;

        if (!path.has_extension())
            return std::unexpected("File has no extension");

        static const std::unordered_set<std::string> supported = {".wav", ".flac", ".ogg", ".mp3", ".aiff"};

        if (!supported.contains(path.extension().string()))
            return std::unexpected("Unsupported audio format: " + path.extension().string());

        return LoadAudioFileToBuffer(filepath, device);
    }

} // namespace Nexus::Audio