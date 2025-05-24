#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/nxpch.hpp"

	#include "SwapchainOpenGL.hpp"

	#include "DeviceBufferOpenGL.hpp"
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

	PixelFormat SwapchainOpenGL::GetColourFormat()
	{
		return PixelFormat::R8_G8_B8_A8_UNorm;
	}

	void SwapchainOpenGL::ResizeIfNecessary()
	{
		Nexus::Point2D<uint32_t> windowSize = m_Window->GetWindowSizeInPixels();

		glCall(glViewport(0, 0, windowSize.X, windowSize.Y));
		glCall(glScissor(0, 0, windowSize.X, windowSize.Y));

		m_SwapchainWidth  = windowSize.X;
		m_SwapchainHeight = windowSize.Y;
	}

	void SwapchainOpenGL::BindAsDrawTarget()
	{
		if (!ViewContext->MakeCurrent())
		{
			throw std::runtime_error("Failed to make context current");
		}
		ResizeIfNecessary();
	}

	void SwapchainOpenGL::Prepare()
	{
	}
}	 // namespace Nexus::Graphics

#endif