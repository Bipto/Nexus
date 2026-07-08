#pragma once

#if defined(_WIN32)
#if defined(NX_PLATFORM_EXPORT)
#define NX_PLATFORM_API __declspec(dllexport)
#else
#define NX_PLATFORM_API __declspec(dllimport)
#endif
#else
#define NX_PLATFORM_API
#endif
