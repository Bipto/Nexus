#pragma once

#include "Nexus-Core/Types.hpp"
#include "AudioBuffer.hpp"
#include "AudioSource.hpp"

#include "Nexus-Core/ApplicationSpecification.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Audio
{
    /// @brief A class representing an abstraction over a graphics API and context
    class AudioDevice
    {
    public:
        /// @brief Virtual destructor used to destroy any necessary data
        virtual ~AudioDevice() {}

        /// @brief A method providing the currently used audio backend
        /// @return An AudioAPI enum representing the current backend
        virtual AudioAPI GetAPI() = 0;

        /// @brief A method that returns an AudioBuffer that is created from a .wav file
        /// @param filepath The path to the .wav file
        /// @return An audio buffer that can be used to create an AudioSource
        virtual Ref<AudioBuffer> CreateAudioBufferFromWavFile(const std::string &filepath) = 0;

        /// @brief A method that returns an AudioBuffer that is created from a .mp3 file
        /// @param filepath The path to the .mp3 file
        /// @return An audio buffer that can be used to create an AudioSource
        virtual Ref<AudioBuffer> CreateAudioBufferFromMP3File(const std::string &filepath) = 0;

        /// @brief A method that creates an AudioSource from an AudioBuffer
        /// @param buffer The buffer to create an AudioSource from
        /// @return The AudioSource that provides a context to play the sound
        virtual Ref<AudioSource> CreateAudioSource(Ref<AudioBuffer> buffer) = 0;

        /// @brief A method that will play a sound effect from an AudioSource
        /// @param source The AudioSource to play the effect from
        virtual void PlaySource(Ref<AudioSource> source) = 0;
    };
}