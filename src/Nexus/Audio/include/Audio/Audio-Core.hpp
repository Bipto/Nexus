#pragma once

#if defined(_WIN32)
	#if defined(NX_AUDIO_EXPORT)
		#define NX_AUDIO_API __declspec(dllexport)
	#else
		#define NX_AUDIO_API __declspec(dllimport)
	#endif
#else
	#define NX_AUDIO_API
#endif
