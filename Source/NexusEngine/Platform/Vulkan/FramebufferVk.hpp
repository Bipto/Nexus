#pragma once

#include "Vk.hpp"
#include "Core/Graphics/Framebuffer.hpp"

namespace Nexus::Graphics
{
    class FramebufferVk : public Framebuffer
    {
    public:
        FramebufferVk(const FramebufferSpecification &spec);
        ~FramebufferVk();

        virtual void *GetColorAttachment(int index = 0) override;
        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;
        virtual void *GetDepthAttachment() override;

    private:
        virtual void Recreate() override;
    };
}