#pragma once

#if defined(NX_PLATFORM_OPENAL)
#endif

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <AL/al.h>
#include <AL/alc.h>
#endif

#if !defined(__EMSCRIPTEN__)
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#else
#define AL_FORMAT_MONO_FLOAT32 0x10010
#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif