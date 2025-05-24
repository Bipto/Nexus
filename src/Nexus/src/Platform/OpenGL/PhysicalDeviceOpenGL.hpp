#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"
	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"

namespace Nexus::Graphics
{
	class PhysicalDeviceOpenGL : public IPhysicalDevice
	{
	  public:
		virtual GL::IOffscreenContext *GetOffscreenContext() = 0;
	};
}	 // namespace Nexus::Graphics

#endif