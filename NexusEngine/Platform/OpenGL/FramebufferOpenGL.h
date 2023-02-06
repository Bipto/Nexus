#pragma once

#include "Core/Graphics/Framebuffer.h"
#include "glad/glad.h"

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
                glDeleteFramebuffers(1, &m_FBO);
            }

            virtual void Bind() override
            {
                glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
            }

            virtual void Unbind() override
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            virtual unsigned int GetColorAttachment() override
            {
                return m_Texture;
            }

            virtual void Resize() override
            {
                glGenFramebuffers(1, &m_FBO);
                glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

                m_Texture = CreateTexture();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
            unsigned int CreateTexture()
            {
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_FramebufferSpecification.Width, m_FramebufferSpecification.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

                return texture;
            }

        private:
            unsigned int m_FBO;
            unsigned int m_Texture;

            Nexus::FramebufferSpecification m_FramebufferSpecification;
    };
}
