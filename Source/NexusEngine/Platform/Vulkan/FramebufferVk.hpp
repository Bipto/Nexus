#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Vk.hpp"
#include "Core/Graphics/Framebuffer.hpp"

namespace Nexus::Graphics
{
    class FramebufferVk : public Framebuffer
    {
    public:
        FramebufferVk(Ref<RenderPass> renderPass);
        ~FramebufferVk();

        virtual void *GetColorAttachment(int index = 0) override;
        virtual const FramebufferSpecification GetFramebufferSpecification() override;
        virtual void SetFramebufferSpecification(const FramebufferSpecification &spec) override;
        virtual void *GetDepthAttachment() override;

    private:
        virtual void Recreate() override;
    };
}

#endif