#pragma once

#include "GL.h"
#include "Core/Graphics/Framebuffer.h"

#include <iostream>
#include <sstream>

namespace Nexus
{
    class FramebufferOpenGL : public Framebuffer
    {
        public:
            FramebufferOpenGL(const Nexus::FramebufferSpecification& spec);
            ~FramebufferOpenGL();

            virtual void Bind() override;
            virtual void Unbind() override;
            virtual void Resize() override;

            virtual int GetColorTextureCount() override;            
            virtual bool HasColorTexture() override;            
            virtual bool HasDepthTexture() override;

            virtual unsigned int GetColorAttachment(int index = 0) override;
            virtual unsigned int GetDepthAttachment() override;
            virtual const FramebufferSpecification GetFramebufferSpecification() override;
            virtual void SetFramebufferSpecification(const FramebufferSpecification& spec) override;

        private:
            void CreateTextures();
            void DeleteTextures();

        private:
            unsigned int m_FBO;
            unsigned int m_Texture;
            std::vector<GLenum> m_Buffers;

            std::vector<unsigned int> m_ColorTextures;
            unsigned int m_DepthTexture;    

            Nexus::FramebufferSpecification m_FramebufferSpecification;
    };
}
