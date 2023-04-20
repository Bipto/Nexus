#include "FramebufferOpenGL.h"

namespace Nexus
{
    FramebufferOpenGL::FramebufferOpenGL(const Nexus::FramebufferSpecification& spec)
    {
        this->m_FramebufferSpecification = spec;
        Resize();
    }

    FramebufferOpenGL::~FramebufferOpenGL()
    {
        DeleteTextures();
    }

    void FramebufferOpenGL::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glDrawBuffers(m_ColorTextures.size(), m_Buffers.data());
    }

    void FramebufferOpenGL::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FramebufferOpenGL::Resize()
    {
        DeleteTextures();

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        CreateTextures();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_Buffers.clear();
        for (int i = 0; i < m_ColorTextures.size(); i++)
        {
            m_Buffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);
        }
    }

    int FramebufferOpenGL::GetColorTextureCount()
    {
        return m_FramebufferSpecification.ColorAttachmentSpecification.Attachments.size(); 
    }
    
    bool FramebufferOpenGL::HasColorTexture()
    {
        return m_FramebufferSpecification.ColorAttachmentSpecification.Attachments.size() > 0;
    }
    
    bool FramebufferOpenGL::HasDepthTexture()
    {
        return m_FramebufferSpecification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None;
    }

    void* FramebufferOpenGL::GetColorAttachment(int index)
    {
        return (void*)m_ColorTextures[index];
    }

    void* FramebufferOpenGL::GetDepthAttachment()
    {
        return (void*)m_DepthTexture;
    }

    const FramebufferSpecification FramebufferOpenGL::GetFramebufferSpecification()
    {
        return this->m_FramebufferSpecification;
    }

    void FramebufferOpenGL::SetFramebufferSpecification(const FramebufferSpecification& spec)
    {
        this->m_FramebufferSpecification = spec;
        Resize();
    }

    void FramebufferOpenGL::CreateTextures()
    {
        m_ColorTextures.clear();

        //color attachments
        for (int i = 0; i < m_FramebufferSpecification.ColorAttachmentSpecification.Attachments.size(); i++)
        {
            auto colorSpec = m_FramebufferSpecification.ColorAttachmentSpecification.Attachments[i];

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_FramebufferSpecification.Width, m_FramebufferSpecification.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SAMPLES, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, 0);
            m_ColorTextures.emplace_back(texture);
        }

        //depth attachment
        if (m_FramebufferSpecification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None)
        {
            glGenTextures(1, &m_DepthTexture);
            glBindTexture(GL_TEXTURE_2D, m_DepthTexture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_FramebufferSpecification.Width, m_FramebufferSpecification.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SAMPLES, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
        }
    }

    void FramebufferOpenGL::DeleteTextures()
    {
        glDeleteFramebuffers(1, &m_FBO);

        for (auto texture : m_ColorTextures)
        {
            glDeleteTextures(1, &texture);
        }

        m_ColorTextures.clear();

        glDeleteTextures(1, &m_DepthTexture);
    }
}