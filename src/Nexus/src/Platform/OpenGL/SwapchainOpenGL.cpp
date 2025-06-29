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

		m_ViewContext = GL::CreateViewContext(window, graphicsDeviceOpenGL);
		m_ViewContext->MakeCurrent();
		SetVSyncState(swapchainSpec.VSyncState);
	}

	SwapchainOpenGL::~SwapchainOpenGL()
	{
	}

	void SwapchainOpenGL::SwapBuffers()
	{
		m_ViewContext->Swap();
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
			m_ViewContext->SetVSync(true);
		}
		else
		{
			m_ViewContext->SetVSync(false);
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

	PixelFormat SwapchainOpenGL::GetDepthFormat()
	{
		return PixelFormat::D24_UNorm_S8_UInt;
	}

	void SwapchainOpenGL::ResizeIfNecessary()
	{
		Nexus::Point2D<uint32_t> windowSize = m_Window->GetWindowSizeInPixels();

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				glCall(context.Viewport(0, 0, windowSize.X, windowSize.Y));
				glCall(context.Scissor(0, 0, windowSize.X, windowSize.Y));
			});

		m_SwapchainWidth  = windowSize.X;
		m_SwapchainHeight = windowSize.Y;
	}

	void SwapchainOpenGL::BindAsDrawTarget()
	{
		if (!m_ViewContext->MakeCurrent())
		{
			throw std::runtime_error("Failed to make context current");
		}
		ResizeIfNecessary();
	}

	GL::IViewContext *SwapchainOpenGL::GetViewContext()
	{
		return m_ViewContext.get();
	}

	void SwapchainOpenGL::Prepare()
	{
	}
}	 // namespace Nexus::Graphics

#endif