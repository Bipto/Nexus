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
        : ISwapchain(swapchainSpec), m_Device(graphicsDevice)
    {
        m_SwapchainWidth = swapchainSpec.Width;
        m_SwapchainHeight = swapchainSpec.Height;

        if (const SurfaceOpenGL *surface = swapchainSpec.Surface.AsDerived<const SurfaceOpenGL>())
        {
            GL::ContextDescription contextDesc = {};
            m_ViewContext = surface->CreateOpenGLContext(graphicsDevice, contextDesc);
        }

        m_ViewContext->MakeCurrent();
        SetPresentMode(m_Description.ImagePresentMode);

        CreateFramebuffer();
    }

    SwapchainOpenGL::~SwapchainOpenGL()
    {
    }

    void SwapchainOpenGL::SwapBuffers(const SwapchainPresentDescription &presentDesc)
    {
        m_ViewContext->Swap(m_Framebuffer->GetColorTextureHandle(0), presentDesc);
    }

    FramebufferHandle SwapchainOpenGL::GetCurrentFramebuffer()
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
        default:
            throw std::runtime_error("Failed to find a valid present mode");
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
            m_SwapchainWidth = width;
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

    FramebufferHandle SwapchainOpenGL::GetFramebuffer()
    {
        return m_Framebuffer;
    }

    void SwapchainOpenGL::CreateFramebuffer()
    {
        IGraphicsDevice *device = m_Device;

        FramebufferTextureCreateDescription framebufferDesc = {};
        framebufferDesc.Width = m_SwapchainWidth;
        framebufferDesc.Height = m_SwapchainHeight;
        framebufferDesc.Samples = m_Description.Samples;
        framebufferDesc.ColourAttachmentFormats = {m_ColourFormat};
        framebufferDesc.DepthAttachmentFormat = m_DepthFormat;
        m_Framebuffer = Utils::CreateFramebuffer(m_Device, framebufferDesc);
    }
} // namespace Nexus::Graphics

#endif