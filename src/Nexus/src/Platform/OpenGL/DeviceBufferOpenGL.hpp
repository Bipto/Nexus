#pragma once

#if defined(NX_PLATFORM_WEBGL)
	#include "Backends/WebGL2/DeviceBufferOpenGL_WebGL.hpp"
#else
	#include "Backends/Default/DeviceBufferOpenGL_Default.hpp"
#endif