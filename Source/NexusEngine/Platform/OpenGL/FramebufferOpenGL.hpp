#pragma once

#include "GL.hpp"
#include "Nexus/Graphics/Framebuffer.hpp"

namespace Nexus::Graphics
{
    class FramebufferOpenGL : public Framebuffer
    {
    public:
        FramebufferOpenGL(RenderPass *renderPass);
        ~FramebufferOpenGL();

        void Bind();
        void Unbind();

        virtual void *GetColorAttachment(int index = 0) override;
        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;
        virtual void *GetDepthAttachment() override { return (void *)m_DepthTexture; }

    private:
        virtual void Recreate() override;
        void CreateTextures();
        void DeleteTextures();

    private:
        unsigned int m_FBO;
        std::vector<GLenum> m_Buffers;

        std::vector<unsigned int> m_ColorTextures;
        unsigned int m_DepthTexture;
    };
}
