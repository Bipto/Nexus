#include "FramebufferVk.hpp"

namespace Nexus::Graphics
{
    FramebufferVk::FramebufferVk(const FramebufferSpecification &spec)
        : Framebuffer(spec)
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
