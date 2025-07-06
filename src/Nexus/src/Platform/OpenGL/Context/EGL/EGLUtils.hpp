#pragma once

#include "egl_include.hpp"

#define CASE_STR(value)                                                                                                                              \
	case value: return #value;
inline const char *eglGetErrorString(EGLint error)
{
	switch (error)
	{
		CASE_STR(EGL_SUCCESS)
		CASE_STR(EGL_NOT_INITIALIZED)
		CASE_STR(EGL_BAD_ACCESS)
		CASE_STR(EGL_BAD_ALLOC)
		CASE_STR(EGL_BAD_ATTRIBUTE)
		CASE_STR(EGL_BAD_CONTEXT)
		CASE_STR(EGL_BAD_CONFIG)
		CASE_STR(EGL_BAD_CURRENT_SURFACE)
		CASE_STR(EGL_BAD_DISPLAY)
		CASE_STR(EGL_BAD_SURFACE)
		CASE_STR(EGL_BAD_MATCH)
		CASE_STR(EGL_BAD_PARAMETER)
		CASE_STR(EGL_BAD_NATIVE_PIXMAP)
		CASE_STR(EGL_BAD_NATIVE_WINDOW)
		CASE_STR(EGL_CONTEXT_LOST)
		default: return "Unknown";
	}
}
#undef CASE_STR

namespace EGL
{
	inline bool HasExtension(EGLDisplay display, const char *name)
	{
		const char *extensions = eglQueryString(display, EGL_EXTENSIONS);
		return extensions && strstr(extensions, name) != nullptr;
	}
}	 // namespace EGL
