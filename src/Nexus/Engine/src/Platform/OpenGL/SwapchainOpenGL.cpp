#if defined(NX_PLATFORM_OPENGL)

	#include "SwapchainOpenGL.hpp"
	#include "DeviceBufferOpenGL.hpp"
	#include "GL.hpp"
	#include "GraphicsDeviceOpenGL.hpp"
	#include "Nexus-Core/Utils/GraphicsUtils.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "Surface/SurfaceOpenGL.hpp"

namespace Nexus::Graphics
{
	SwapchainOpenGL::SwapchainOpenGL(const SwapchainDescription &swapchainSpec, GraphicsDeviceOpenGL *graphicsDevice)
		: ISwapchain(swapchainSpec),
		  m_Device(graphicsDevice)
	{
		m_SwapchainWidth  = swapchainSpec.Width;
		m_SwapchainHeight = swapchainSpec.Height;

		if (auto surface = std::dynamic_pointer_cast<SurfaceOpenGL>(swapchainSpec.Surface))
		{
			GL::ContextDescription contextDesc = {};
			m_ViewContext					   = surface->CreateOpenGLContext(graphicsDevice, contextDesc);
		}

		// m_ViewContext = GL::CreateViewContext(window, graphicsDevice);

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
		m_ViewContext->MakeCurrent();

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.PushDebugGroup)
				{
					// context.PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "SwapchainOpenGL::SwapBuffers");
				}

				m_ViewContext->Swap(m_Framebuffer->GetColorTextureHandle(0), presentDesc);

				if (context.PopDebugGroup)
				{
					// context.PopDebugGroup();
				}
			});

		GL::SetCurrentContext(m_Device->GetOffscreenContext());
	}

	Ref<IFramebuffer> SwapchainOpenGL::GetCurrentFramebuffer()
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

	std::pair<uint32_t, uint32_t> SwapchainOpenGL::GetSize()
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

	std::expected<void, std::string> SwapchainOpenGL::Resize(uint32_t width, uint32_t height)
	{
		if (width != m_SwapchainWidth || height != m_SwapchainHeight)
		{
			m_SwapchainWidth  = width;
			m_SwapchainHeight = height;

			CreateFramebuffer();
		}

		return std::expected<void, std::string>();
	}

	void SwapchainOpenGL::BindAsDrawTarget()
	{
		if (!m_ViewContext->MakeCurrent())
		{
			throw std::runtime_error("Failed to make context current");
		}
	}

	GL::IViewContext *SwapchainOpenGL::GetViewContext()
	{
		return m_ViewContext.get();
	}

	Ref<IFramebuffer> SwapchainOpenGL::GetFramebuffer()
	{
		return m_Framebuffer;
	}

	void SwapchainOpenGL::CreateFramebuffer()
	{
		IGraphicsDevice *device = m_Device;

		FramebufferTextureCreateDescription framebufferDesc = {};
		framebufferDesc.Width								= m_SwapchainWidth;
		framebufferDesc.Height								= m_SwapchainHeight;
		framebufferDesc.Samples								= m_Description.Samples;
		framebufferDesc.ColourAttachmentFormats				= {m_ColourFormat};
		framebufferDesc.DepthAttachmentFormat				= m_DepthFormat;
		m_Framebuffer = std::dynamic_pointer_cast<FramebufferOpenGL>(Utils::CreateFramebuffer(m_Device, framebufferDesc));
	}
}	 // namespace Nexus::Graphics

#endif