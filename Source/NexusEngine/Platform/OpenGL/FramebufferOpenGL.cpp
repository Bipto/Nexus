#if defined(NX_PLATFORM_OPENGL)

#include "FramebufferOpenGL.hpp"
#include "GraphicsDeviceOpenGL.hpp"
#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
    FramebufferOpenGL::FramebufferOpenGL(const FramebufferSpecification &spec, GraphicsDeviceOpenGL *graphicsDevice)
        : Framebuffer(spec), m_Device(graphicsDevice)
    {
        Recreate();
    }

    FramebufferOpenGL::~FramebufferOpenGL()
    {
        DeleteTextures();
    }

    void FramebufferOpenGL::BindAsRenderTarget()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        auto width = m_Specification.Width;
        auto height = m_Specification.Height;
        glViewport(0, 0, width, height);
        glScissor(0, 0, width, height);
    }

    void FramebufferOpenGL::BindAsReadBuffer(uint32_t texture)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + texture);
    }

    void FramebufferOpenGL::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FramebufferOpenGL::Recreate()
    {
        DeleteTextures();

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        CreateTextures();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    const FramebufferSpecification FramebufferOpenGL::GetFramebufferSpecification()
    {
        return m_Specification;
    }

    void FramebufferOpenGL::SetFramebufferSpecification(const FramebufferSpecification &spec)
    {
        m_Specification = spec;
        Recreate();
    }

    Texture *FramebufferOpenGL::GetColorTexture(uint32_t index)
    {
        return m_ColorAttachments.at(index);
    }

    Texture *FramebufferOpenGL::GetDepthTexture()
    {
        return m_DepthAttachment;
    }

    void FramebufferOpenGL::CreateTextures()
    {
        m_ColorAttachments.clear();

        for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
        {
            const auto &colorAttachmentSpec = m_Specification.ColorAttachmentSpecification.Attachments[i];

            if (colorAttachmentSpec.TextureFormat == PixelFormat::None)
            {
                NX_ASSERT(0, "Pixel format cannot be PixelFormat::None for a color attachment");
            }

            Nexus::Graphics::TextureSpecification spec;
            spec.Width = m_Specification.Width;
            spec.Height = m_Specification.Height;
            spec.Format = colorAttachmentSpec.TextureFormat;
            spec.NumberOfChannels = 4;
            spec.Samples = m_Specification.Samples;
            spec.Usage = {TextureUsage::Sampled, TextureUsage::RenderTarget};
            auto texture = (TextureOpenGL *)m_Device->CreateTexture(spec);
            m_ColorAttachments.push_back(texture);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetNativeHandle(), 0);
        }

        if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
        {
            Nexus::Graphics::TextureSpecification spec;
            spec.Width = m_Specification.Width;
            spec.Height = m_Specification.Height;
            spec.Format = m_Specification.DepthAttachmentSpecification.DepthFormat;
            spec.NumberOfChannels = 2;
            spec.Samples = m_Specification.Samples;
            spec.Usage = {TextureUsage::DepthStencil};
            m_DepthAttachment = m_Device->CreateTexture(spec);

            auto glTexture = (TextureOpenGL *)m_DepthAttachment;

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, glTexture->GetNativeHandle(), 0);
        }
    }

    void FramebufferOpenGL::DeleteTextures()
    {
    }
}

#endif