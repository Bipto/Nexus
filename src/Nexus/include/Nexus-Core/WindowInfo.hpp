#pragma once

#include "Nexus-Core/nxpch.hpp"

#if defined(NX_PLATFORM_WINDOWS)
	#include "Platform/Windows/WindowsInclude.hpp"
#elif defined(NX_PLATFORM_LINUX)
	#include "Platform/X11/X11Include.hpp"
#elif defined(NX_PLATFORM_ANDROID)
	#include <jni.h>
	#include <android/native_window.h>
	#include <android/native_window_jni.h>
#endif

namespace Nexus
{
	struct NativeWindowInfo
	{
#if defined(NX_PLATFORM_WINDOWS)
		HWND	  hwnd	   = {};
		HDC		  hdc	   = {};
		HINSTANCE instance = {};
#elif defined(NX_PLATFORM_LINUX)
		Display *display = {};
		int		 screen	 = {};
		Window	 window	 = {};
#elif defined(NX_PLATFORM_WEB)
		std::string canvasId = {};
#elif defined(NX_PLATFORM_ANDROID)
		ANativeWindow *nativeWindow = {};
#endif
	};
}	 // namespace Nexus