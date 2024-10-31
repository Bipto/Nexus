#if defined(NX_PLATFORM_OPENGL)

	#include "SwapchainOpenGL.hpp"

	#include "BufferOpenGL.hpp"
	#include "GL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"

namespace Nexus::Graphics
{
	SwapchainOpenGL::SwapchainOpenGL(Window *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice)
		: Swapchain(swapchainSpec),
		  m_Window(window),
		  m_VsyncState(swapchainSpec.VSyncState)
	{
		m_SwapchainWidth  = m_Window->GetWindowSize().X;
		m_SwapchainHeight = m_Window->GetWindowSize().Y;

		GraphicsDeviceOpenGL *graphicsDeviceOpenGL = (GraphicsDeviceOpenGL *)graphicsDevice;

		m_FBO = GL::CreateFBO(window, graphicsDeviceOpenGL->GetPBuffer());
		m_FBO->MakeCurrent();
		SetVSyncState(swapchainSpec.VSyncState);
	}

	SwapchainOpenGL::~SwapchainOpenGL()
	{
	}

	void SwapchainOpenGL::SwapBuffers()
	{
		BindAsRenderTarget();
		m_FBO->Swap();
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
			m_FBO->SetVSync(true);
		}
		else
		{
			m_FBO->SetVSync(false);
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

		glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer));
		glCall(glViewport(0, 0, w, h));
		glCall(glScissor(0, 0, w, h));

		m_SwapchainWidth  = w;
		m_SwapchainHeight = h;
	}

	void SwapchainOpenGL::BindAsRenderTarget()
	{
		m_FBO->MakeCurrent();

		glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer));
		ResizeIfNecessary();
	}

	void SwapchainOpenGL::BindAsDrawTarget()
	{
		m_FBO->MakeCurrent();
		glCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Backbuffer));
	}

	void SwapchainOpenGL::Prepare()
	{
	}
}	 // namespace Nexus::Graphics

#endif