#pragma once

#if defined(NX_PLATFORM_WEBGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "PhysicalDeviceWebGL.hpp"
	#include "Platform/OpenGL/GL.hpp"

namespace Nexus::GL
{
	bool LoadOpenGL()
	{
		return true;
	}

	std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> LoadAvailablePhysicalDevices()
	{
		std::shared_ptr<Graphics::PhysicalDeviceWebGL> physicalDevice = std::make_unique<Graphics::PhysicalDeviceWebGL>("WebGL2");
		return {physicalDevice};
	}
}	 // namespace Nexus::GL

#endif