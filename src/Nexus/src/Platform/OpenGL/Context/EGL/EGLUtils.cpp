#include "Platform/OpenGL/GL.hpp"

#include "PhysicalDeviceEGL.hpp"
#include "egl_include.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::GL
{
	bool LoadOpenGL()
	{
		int version = gladLoaderLoadEGL(EGL_DEFAULT_DISPLAY);
		int major	= GLAD_VERSION_MAJOR(version);
		int minor	= GLAD_VERSION_MINOR(version);

		if (!EGL_EXT_device_enumeration || !EGL_EXT_device_base || !EGL_EXT_device_query)
		{
			throw std::runtime_error("Extensions are not available");
		}

		return (bool)version;
	}

	std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> LoadAvailablePhysicalDevices()
	{
		std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> physicalDevices;

		physicalDevices.push_back(std::make_unique<Graphics::PhysicalDeviceEGL>());

		// const size_t MaxDevices = 10;
		// EGLDeviceEXT devices[MaxDevices];
		// EGLint		 numDevices;

		// if (eglQueryDevicesEXT(MaxDevices, devices, &numDevices))
		//{
		//	EGLDeviceEXT device	 = devices[0];
		//	EGLDisplay	 display = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, devices[0], nullptr);

		//	if (display == EGL_NO_DISPLAY)
		//	{
		//		fprintf(stderr, "Failed to get EGLDisplay from device\n");
		//	}
		//	else
		//	{
		//		int version = gladLoaderLoadEGL(display);
		//		int major	= GLAD_VERSION_MAJOR(version);
		//		int minor	= GLAD_VERSION_MINOR(version);

		//		if (!eglInitialize(display, NULL, NULL))
		//		{
		//			fprintf(stderr, "Failed to initialize EGLDisplay\n");
		//		}
		//		else
		//		{
		//			printf("EGLDisplay initialized successfully\n");
		//		}
		//	}
		//}

		return physicalDevices;
	}
}	 // namespace Nexus::GL
