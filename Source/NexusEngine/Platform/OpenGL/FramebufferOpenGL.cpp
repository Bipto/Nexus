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
        /* m_ColorTextures.clear();

        // color attachments
        for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
        {
            auto colorSpec = m_Specification.ColorAttachmentSpecification.Attachments[i];

            unsigned int texture;
            glGenTextures(1, &texture);

            auto textureFormat = GL::GetPixelType(colorSpec.TextureFormat);

#if !defined(NX_PLATFORM_WEBGL2)
            if (m_Specification.Samples != SampleCount::SampleCount1)
            {
                uint32_t samples = GetSampleCount(m_Specification.Samples);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, textureFormat, m_Specification.Width, m_Specification.Height, GL_FALSE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, texture);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexStorage2D(GL_TEXTURE_2D, 1, textureFormat, m_Specification.Width, m_Specification.Height);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, 0);
            }
#else
            glBindTexture(GL_TEXTURE_2D, texture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexStorage2D(GL_TEXTURE_2D, 1, textureFormat, m_Specification.Width, m_Specification.Height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, 0);
#endif

            m_ColorTextures.emplace_back(texture);
    }

    // depth attachment
    if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
    {
        glGenTextures(1, &m_DepthTexture);

#if !defined(NX_PLATFORM_WEBGL2)
        if (m_Specification.Samples != SampleCount::SampleCount1)
        {
            uint32_t samples = GetSampleCount(m_Specification.Samples);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthTexture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, GL_FALSE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthTexture, 0);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexStorage2D(GL_FRAMEBUFFER, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
        }
#else
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexStorage2D(GL_FRAMEBUFFER, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
#endif
    } */

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
        /* glDeleteFramebuffers(1, &m_FBO);

        for (auto texture : m_ColorTextures)
        {
            glDeleteTextures(1, &texture);
        }

        m_ColorTextures.clear();

        glDeleteTextures(1, &m_DepthTexture);*/
    }
}

#endif