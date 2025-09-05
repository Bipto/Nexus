#include "Platform/OpenGL/GL.hpp"

#include "PhysicalDeviceEGL.hpp"
#include "egl_include.hpp"

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Platform.hpp"

#include "EGLUtils.hpp"

namespace Nexus::GL
{
	bool LoadOpenGL()
	{
		// load initial EGL functions to enable retrieving devices
		int version = gladLoaderLoadEGL(EGL_DEFAULT_DISPLAY);
		int major	= GLAD_VERSION_MAJOR(version);
		int minor	= GLAD_VERSION_MINOR(version);

		return (bool)version;
	}

	std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> LoadAvailablePhysicalDevices(bool debug)
	{
		std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> physicalDevices;

#if !defined(NX_PLATFORM_ANDROID)
		Nexus::WindowDescription windowSpec {};
		windowSpec.Width	 = 1;
		windowSpec.Height	 = 1;
		windowSpec.Resizable = false;
		IWindow *window		 = Platform::CreatePlatformWindow(windowSpec);
		window->Hide();

		NativeWindowInfo windowInfo = window->GetNativeWindowInfo();
		EGLDisplay		 display	= eglGetDisplay(windowInfo.display);
#else
		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif

		physicalDevices.push_back(std::make_unique<Graphics::PhysicalDeviceEGL>(debug, display));

		return physicalDevices;
	}
}