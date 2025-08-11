#pragma once

#if defined(NX_PLATFORM_OPENAL)
#endif

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include "Nexus-Core/Audio/AudioTypes.hpp"

#if defined(__EMSCRIPTEN__)
	#define AL_FORMAT_MONO_FLOAT32	 0x10010
	#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif

namespace Nexus::AL
{
	ALenum			   GetOpenALFormat(Audio::AudioFormat format);
	Audio::SourceType  GetSourceType(ALint type);
	Audio::SourceState GetSourceState(ALint state);
}	 // namespace Nexus::AL