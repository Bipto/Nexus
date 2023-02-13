#pragma once

#include "Core/Graphics/Framebuffer.h"
#include "glad/glad.h"

#include <iostream>
#include <sstream>

namespace Nexus
{
    class FramebufferOpenGL : public Framebuffer
    {
        public:
            FramebufferOpenGL(const Nexus::FramebufferSpecification& spec)
            {
                this->m_FramebufferSpecification = spec;
                Resize();
            }

            ~FramebufferOpenGL()
            {
                DeleteTextures();
            }

            virtual void Bind() override
            {
                glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
                glDrawBuffers(m_ColorTextures.size(), m_Buffers.data());
            }

            virtual void Unbind() override
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            virtual void Resize() override
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

            virtual int GetColorTextureCount()
            {
                return m_FramebufferSpecification.ColorAttachmentSpecification.Attachments.size(); 
            }
            
            virtual bool HasColorTexture()
            {
                return m_FramebufferSpecification.ColorAttachmentSpecification.Attachments.size() > 0;
            }
            
            virtual bool HasDepthTexture()
            {
                return m_FramebufferSpecification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None;
            }

            virtual unsigned int GetColorAttachment(int index = 0)
            {
                return m_ColorTextures[index];
            }

            virtual unsigned int GetDepthAttachment()
            {
                return m_DepthTexture;
            }

            virtual const FramebufferSpecification GetFramebufferSpecification() override
            {
                return this->m_FramebufferSpecification;
            }

            virtual void SetFramebufferSpecification(const FramebufferSpecification& spec) override
            {
                this->m_FramebufferSpecification = spec;
                Resize();
            }

        private:
            void CreateTextures()
            {
                m_ColorTextures.clear();

                for (int i = 0; i < m_FramebufferSpecification.ColorAttachmentSpecification.Attachments.size(); i++)
                {
                    auto colorSpec = m_FramebufferSpecification.ColorAttachmentSpecification.Attachments[i];

                    unsigned int texture;
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D, texture);

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_FramebufferSpecification.Width, m_FramebufferSpecification.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, 0);
                    m_ColorTextures.emplace_back(texture);
                }
            }

            void DeleteTextures()
            {
                glDeleteFramebuffers(1, &m_FBO);
                /* glDeleteTextures(1, &m_Texture); */

                /* for (int i = 1; i <= m_FramebufferSpecification.ColorAttachmentSpecification.Attachments.size(); i++)
                {
                    auto texture = m_ColorTextures[i];
                    glDeleteTextures(1, &texture);
                } */

                for (auto texture : m_ColorTextures)
                {
                    glDeleteTextures(1, &texture);
                }

                m_ColorTextures.clear();

                glDeleteTextures(1, &m_DepthTexture);
            }

        private:
            unsigned int m_FBO;
            unsigned int m_Texture;
            std::vector<GLenum> m_Buffers;

            std::vector<unsigned int> m_ColorTextures;
            unsigned int m_DepthTexture;    

            Nexus::FramebufferSpecification m_FramebufferSpecification;
    };
}
