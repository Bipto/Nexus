#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/nxpch.hpp"

	#include "SwapchainOpenGL.hpp"

	#include "BufferOpenGL.hpp"
	#include "GL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	SwapchainOpenGL::SwapchainOpenGL(IWindow *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice)
		: Swapchain(swapchainSpec),
		  m_Window(window),
		  m_VsyncState(swapchainSpec.VSyncState)
	{
		m_SwapchainWidth  = m_Window->GetWindowSize().X;
		m_SwapchainHeight = m_Window->GetWindowSize().Y;

		GraphicsDeviceOpenGL *graphicsDeviceOpenGL = (GraphicsDeviceOpenGL *)graphicsDevice;

		ViewContext = GL::CreateViewContext(window, graphicsDeviceOpenGL);
		ViewContext->MakeCurrent();
		SetVSyncState(swapchainSpec.VSyncState);
	}

	SwapchainOpenGL::~SwapchainOpenGL()
	{
	}

	void SwapchainOpenGL::SwapBuffers()
	{
		ViewContext->Swap();
		ResizeIfNecessary();
	}

	VSyncState SwapchainOpenGL::GetVsyncState()
	{
		return m_VsyncState;
	}

	void SwapchainOpenGL::SetVSyncState(VSyncState vsyncState)
	{
		m_VsyncState = vsyncState;

		if (vsyncState == VSyncState::Enabled)
		{
			ViewContext->SetVSync(true);
		}
		else
		{
			ViewContext->SetVSync(false);
		}
	}

	Nexus::Point2D<uint32_t> SwapchainOpenGL::GetSize()
	{
		return {m_SwapchainWidth, m_SwapchainHeight};
	}

	void SwapchainOpenGL::ResizeIfNecessary()
	{
		int w, h;
		SDL_GetWindowSizeInPixels(m_Window->GetSDLWindowHandle(), &w, &h);

		glCall(glViewport(0, 0, w, h));
		glCall(glScissor(0, 0, w, h));

		m_SwapchainWidth  = w;
		m_SwapchainHeight = h;
	}

	void SwapchainOpenGL::BindAsDrawTarget()
	{
		ViewContext->MakeCurrent();
		ResizeIfNecessary();
	}

	void SwapchainOpenGL::Prepare()
	{
	}
}	 // namespace Nexus::Graphics

#endif