#pragma once

namespace Nexus
{
#if defined(NX_PLATFORM_WINDOWS)
	#include "Platform/Windows/WindowsInclude.hpp"
	struct WindowInfo
	{
		HWND hwnd = {};
	};
#elif defined(NX_PLATFORM_LINUX)
	#include "Platform/X11/X11Include.hpp"
	struct WindowInfo
	{
		Display *display = {};
		Window	 window	 = {};
	};
#elif defined(NX_PLATFORM_WEB)
	#include <string>
	struct WindowInfo
	{
		const char *id = {};
	};
#endif
}	 // namespace Nexus