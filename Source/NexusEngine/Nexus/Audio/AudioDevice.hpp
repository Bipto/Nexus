#pragma once

#include "Nexus/Types.hpp"
#include "AudioBuffer.hpp"
#include "AudioSource.hpp"

#include <string>

namespace Nexus::Audio
{
    /// @brief An enum representing the different audio backends that are available
    enum AudioAPI
    {
        OpenAL
    };

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
        virtual AudioBuffer *CreateAudioBufferFromWavFile(const std::string &filepath) = 0;

        /// @brief A method that returns an AudioBuffer that is created from a .mp3 file
        /// @param filepath The path to the .mp3 file
        /// @return An audio buffer that can be used to create an AudioSource
        virtual AudioBuffer *CreateAudioBufferFromMP3File(const std::string &filepath) = 0;

        /// @brief A method that creates an AudioSource from an AudioBuffer
        /// @param buffer The buffer to create an AudioSource from
        /// @return The AudioSource that provides a context to play the sound
        virtual AudioSource *CreateAudioSource(AudioBuffer *buffer) = 0;

        /// @brief A method that will play a sound effect from an AudioSource
        /// @param source The AudioSource to play the effect from
        virtual void PlaySource(AudioSource *source) = 0;
    };
}