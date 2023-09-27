#pragma once

#include "AudioBuffer.hpp"

namespace Nexus::Audio
{
    /// @brief A pure virtual class representing a buffer of audio data
    class AudioBuffer
    {
    public:
        /// @brief A virtual destructor to allow resources to be cleaned up
        virtual ~AudioBuffer() {}

        /// @brief A pure virtual method to provide access to the raw buffer
        /// @return A handle to the native buffer
        virtual void *GetHandle() = 0;
    };
}