#pragma once

#include <memory>

#include "Audio/AudioDevice.hpp"

namespace Nexus::Audio
{
    class OpenAL
    {
      public:
        static std::shared_ptr<AudioDevice> CreateDevice();
    };
} // namespace Nexus::Audio