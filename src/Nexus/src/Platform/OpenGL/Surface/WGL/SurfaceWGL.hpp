#pragma once

#include "Platform/OpenGL/ContextSpecification.hpp"
#include "Platform/OpenGL/Surface/SurfaceOpenGL.hpp"

namespace Nexus::Graphics
{
	class SurfaceWGL final : public SurfaceOpenGL
	{
	  public:
		SurfaceWGL(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance, const GraphicsDeviceOpenGL *device);
		~SurfaceWGL() override = default;
		std::unique_ptr<GL::IViewContext> CreateOpenGLContext(GraphicsDeviceOpenGL *device, const GL::ContextDescription &desc) const final;

	  private:
		uintptr_t					m_HWND		= {};
		uintptr_t					m_HDC		= {};
		uintptr_t					m_Hinstance = 0;
		const GraphicsDeviceOpenGL *m_Device	= nullptr;
	};
}	 // namespace Nexus::Graphics