#pragma once

#include "AudioBuffer.h"

namespace Nexus
{
    /// @brief A pure virtual class representing a buffer of audio data
    class AudioBuffer
    {
    public:
        /// @brief A pure virtual method to provide access to the raw buffer
        /// @return A handle to the native buffer
        virtual void *GetHandle() = 0;
    };
}