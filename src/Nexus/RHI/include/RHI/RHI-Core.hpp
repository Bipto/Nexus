#pragma once

#if defined(_WIN32)
#if defined(NX_RHI_EXPORT)
#define NX_RHI_API __declspec(dllexport)
#else
#define NX_RHI_API __declspec(dllimport)
#endif
#else
#define NX_RHI_API
#endif