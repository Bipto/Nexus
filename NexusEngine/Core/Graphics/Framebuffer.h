#pragma once

namespace Nexus
{
    struct FramebufferSpecification
    {
        int Width = 1280;
        int Height = 720;
    };

    class Framebuffer
    {
        public:
            virtual void Bind() = 0;
            virtual void Unbind() = 0;
            virtual unsigned int GetColorAttachment() = 0;
            virtual void Resize() = 0;

            virtual const FramebufferSpecification GetFramebufferSpecification() = 0;
            virtual void SetFramebufferSpecification(const FramebufferSpecification& spec) = 0;
    };
}