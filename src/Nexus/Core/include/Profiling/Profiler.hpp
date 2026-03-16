#pragma once

#define NX_PROFILING_ENABLE 1
#if defined(NX_PROFILING_ENABLE) && defined(NX_TRACY_AVAILABLE)
	#include <tracy/Tracy.hpp>

	#define NX_PROFILE_FUNCTION()  ZoneScoped;
	#define NX_PROFILE_SCOPE(name) ZoneScopedN(#name);
	#define NX_MARK_FRAME_END()	   FrameMark;

#else
	#define NX_PROFILE_FUNCTION()
	#define NX_PROFILE_SCOPE(name)
	#define NX_MARK_FRAME_END()
#endif