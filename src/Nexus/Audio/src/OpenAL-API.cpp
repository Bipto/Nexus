#include "OpenAL/OpenAL-API.hpp"

#if defined(NX_PLATFORM_OPENAL)
#include "AudioDeviceOpenAL.hpp"
#endif

namespace Nexus::Audio
{
    std::shared_ptr<Nexus::Audio::AudioDevice> OpenAL::CreateDevice()
    {
#if defined(NX_PLATFORM_OPENAL)
        return std::make_shared<AudioDeviceOpenAL>();
#else
        return nullptr;

#endif
    }
} // namespace Nexus::Audio