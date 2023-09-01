#if defined(NX_PLATFORM_VULKAN)

#include "FramebufferVk.hpp"

namespace Nexus::Graphics
{
    FramebufferVk::FramebufferVk(Ref<RenderPass> renderPass)
        : Framebuffer(renderPass)
    {
    }

    FramebufferVk::~FramebufferVk()
    {
    }

    void *FramebufferVk::GetColorAttachment(int index)
    {
        return nullptr;
    }

    const FramebufferSpecification FramebufferVk::GetFramebufferSpecification()
    {
        return FramebufferSpecification();
    }

    void FramebufferVk::SetFramebufferSpecification(const FramebufferSpecification &spec)
    {
    }

    void *FramebufferVk::GetDepthAttachment()
    {
        return nullptr;
    }

    void FramebufferVk::Recreate()
    {
    }
}

#endif