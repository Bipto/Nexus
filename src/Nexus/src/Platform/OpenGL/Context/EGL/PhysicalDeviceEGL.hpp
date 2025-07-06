#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "OffscreenContextEGL.hpp"
#include "Platform/OpenGL/Context/EGL/egl_include.hpp"
#include "Platform/OpenGL/PhysicalDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	class PhysicalDeviceEGL final : public PhysicalDeviceOpenGL
	{
	  public:
		PhysicalDeviceEGL(bool debug, EGLDisplay display);
		virtual ~PhysicalDeviceEGL();
		const std::string			  &GetDeviceName() const final;
		virtual GL::IOffscreenContext *GetOffscreenContext() final;

	  private:
		std::unique_ptr<GL::OffscreenContextEGL> m_OffscreenContext = nullptr;
		std::string								 m_DeviceName		= {};
	};
}	 // namespace Nexus::Graphics