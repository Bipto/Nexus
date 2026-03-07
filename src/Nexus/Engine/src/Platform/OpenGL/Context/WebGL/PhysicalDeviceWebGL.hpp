#pragma once

#if defined(NX_PLATFORM_WEBGL)

	#include "Platform/OpenGL/PhysicalDeviceOpenGL.hpp"
	#include "OffscreenContextWebGL.hpp"
	#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class PhysicalDeviceWebGL : public PhysicalDeviceOpenGL
	{
	  public:
		PhysicalDeviceWebGL(const std::string &deviceName);
		virtual ~PhysicalDeviceWebGL();
		const std::string					 &GetDeviceName() const final;
		virtual GL::IOffscreenContext		 *GetOffscreenContext() final;

	  private:
		std::unique_ptr<GL::OffscreenContextWebGL> m_OffscreenContext = nullptr;
		std::string								   m_DeviceName		  = {};
		PhysicalDeviceFeatures					   m_Features		  = {};
		PhysicalDeviceLimits					   m_Limits			  = {};
	};
}	 // namespace Nexus::Graphics

#endif