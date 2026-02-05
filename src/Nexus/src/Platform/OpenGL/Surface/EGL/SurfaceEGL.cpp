#include "SurfaceEGL.hpp"

#include "Platform/OpenGL/Context/EGL/OffscreenContextEGL.hpp"
#include "Platform/OpenGL/Context/EGL/ViewContextEGL.hpp"
#include "Platform/OpenGL/GL.hpp"

namespace Nexus::Graphics
{
	SurfaceEGL::SurfaceEGL(uintptr_t display, uint32_t screen, uint32_t window, const GraphicsDeviceOpenGL *device)
		: m_Display(display),
		  m_Screen(screen),
		  m_Window(window),
		  m_Device(device)
	{
	}

	std::unique_ptr<GL::IViewContext> SurfaceEGL::CreateOpenGLContext(GraphicsDeviceOpenGL *device, const GL::ContextDescription &desc) const
	{
		GL::OffscreenContextEGL *pbufferEGL = (GL::OffscreenContextEGL *)device->GetOffscreenContext();
		Display					*display	= reinterpret_cast<Display *>(m_Display);
		int						 screen		= static_cast<int>(m_Screen);
		Window					 window		= static_cast<Window>(m_Window);

		auto context = std::make_unique<GL::ViewContextEGL>(display, window, pbufferEGL, desc);
		return context;
	}
}	 // namespace Nexus::Graphics