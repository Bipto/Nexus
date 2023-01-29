#pragma once

#include "Core/Graphics/Framebuffer.h"
#include "glad/glad.h"

namespace Nexus
{
    class FramebufferOpenGL : public Framebuffer
    {
        public:
            FramebufferOpenGL()
            {
                glGenFramebuffers(1, &m_FBO);
                glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

                m_Texture = CreateTexture();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

        private:
            unsigned int CreateTexture()
            {
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

                return texture;
            }

        private:
            unsigned int m_FBO;
            unsigned int m_Texture;
    };
}
