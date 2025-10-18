#if defined(NX_PLATFORM_OPENGL)

	#include "SwapchainOpenGL.hpp"
	#include "DeviceBufferOpenGL.hpp"
	#include "GL.hpp"
	#include "GraphicsDeviceOpenGL.hpp"
	#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	SwapchainOpenGL::SwapchainOpenGL(IWindow *window, const SwapchainDescription &swapchainSpec, GraphicsDeviceOpenGL *graphicsDevice)
		: Swapchain(swapchainSpec),
		  m_Window(window),
		  m_Device(graphicsDevice)
	{
		m_SwapchainWidth  = m_Window->GetWindowSize().X;
		m_SwapchainHeight = m_Window->GetWindowSize().Y;

		m_ViewContext = GL::CreateViewContext(window, graphicsDevice);
		m_ViewContext->MakeCurrent();

		SetPresentMode(m_Description.ImagePresentMode);

		GL::SetCurrentContext(graphicsDevice->GetOffscreenContext());
		CreateFramebuffer();
	}

	SwapchainOpenGL::~SwapchainOpenGL()
	{
		GL::SetCurrentContext(m_Device->GetOffscreenContext());
	}

	void SwapchainOpenGL::SwapBuffers(const SwapchainPresentDescription &presentDesc)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.PushDebugGroup)
				{
					context.PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "SwapchainOpenGL::SwapBuffers");
				}

				m_ViewContext->Swap(std::dynamic_pointer_cast<TextureOpenGL>(m_Framebuffer->GetColorTextureHandle(0)), presentDesc);

				if (context.PopDebugGroup)
				{
					context.PopDebugGroup();
				}
			});

		ResizeIfNecessary();
	}

	Ref<Framebuffer> SwapchainOpenGL::GetCurrentFramebuffer()
	{
		return m_Framebuffer;
	}

	void SwapchainOpenGL::SetPresentMode(PresentMode presentMode)
	{
		m_Description.ImagePresentMode = presentMode;

		switch (presentMode)
		{
			case Graphics::PresentMode::Immediate:
			{
				m_ViewContext->SetVSync(false);
				break;
			}
			case Graphics::PresentMode::Mailbox:
			case Graphics::PresentMode::Fifo:
			case Graphics::PresentMode::FifoRelaxed:
			{
				m_ViewContext->SetVSync(true);
				break;
			}
			default: throw std::runtime_error("Failed to find a valid present mode");
		}
	}

	Nexus::Point2D<uint32_t> SwapchainOpenGL::GetSize()
	{
		return {m_SwapchainWidth, m_SwapchainHeight};
	}

	PixelFormat SwapchainOpenGL::GetColourFormat()
	{
		return m_ColourFormat;
	}

	PixelFormat SwapchainOpenGL::GetDepthFormat()
	{
		return m_DepthFormat;
	}

	void SwapchainOpenGL::ResizeIfNecessary()
	{
		Nexus::Point2D<uint32_t> windowSize = m_Window->GetWindowSizeInPixels();

		if (windowSize.X != m_SwapchainWidth || windowSize.Y != m_SwapchainHeight)
		{
			m_SwapchainWidth  = windowSize.X;
			m_SwapchainHeight = windowSize.Y;

			CreateFramebuffer();
		}
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

	IWindow *SwapchainOpenGL::GetWindow()
	{
		return m_Window;
	}

	Ref<Framebuffer> SwapchainOpenGL::GetFramebuffer()
	{
		return m_Framebuffer;
	}

	void SwapchainOpenGL::CreateFramebuffer()
	{
		GraphicsDevice *device = m_Device;

		FramebufferTextureCreateDescription framebufferDesc = {};
		framebufferDesc.Width								= m_SwapchainWidth;
		framebufferDesc.Height								= m_SwapchainHeight;
		framebufferDesc.Samples								= m_Description.Samples;
		framebufferDesc.ColourAttachmentFormats				= {m_ColourFormat};
		framebufferDesc.DepthAttachmentFormat				= m_DepthFormat;
		m_Framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(device->CreateFramebuffer(framebufferDesc));
	}
}	 // namespace Nexus::Graphics

#endif