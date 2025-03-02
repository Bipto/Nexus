#pragma once

#if defined(NX_PLATFORM_OPENAL)
#endif

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#if defined(__EMSCRIPTEN__)
	#define AL_FORMAT_MONO_FLOAT32	 0x10010
	#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif