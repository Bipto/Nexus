#if defined(NX_PLATFORM_VULKAN)

#include "FramebufferVk.hpp"

namespace Nexus::Graphics
{
FramebufferVk::FramebufferVk(const FramebufferSpecification &spec, GraphicsDeviceVk *device) : Framebuffer(spec), m_Device(device)
{
    m_Specification = spec;
    Recreate();
}

FramebufferVk::~FramebufferVk()
{
}

const FramebufferSpecification FramebufferVk::GetFramebufferSpecification()
{
    return m_Specification;
}

void FramebufferVk::SetFramebufferSpecification(const FramebufferSpecification &spec)
{
    m_Specification = spec;
    Recreate();
}

Ref<Texture2D> FramebufferVk::GetColorTexture(uint32_t index)
{
    return m_ColorAttachments.at(index);
}

Ref<Texture2D> FramebufferVk::GetDepthTexture()
{
    return m_DepthAttachment;
}

Ref<Texture2D_Vk> FramebufferVk::GetVulkanColorTexture(uint32_t index)
{
    return m_ColorAttachments.at(index);
}

Ref<Texture2D_Vk> FramebufferVk::GetVulkanDepthTexture()
{
    return m_DepthAttachment;
}

void FramebufferVk::Recreate()
{
    CreateColorTargets();
    CreateDepthTargets();
}

void FramebufferVk::CreateColorTargets()
{
    m_ColorAttachments.clear();

    for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
    {
        const auto &colorAttachmentSpec = m_Specification.ColorAttachmentSpecification.Attachments.at(i);

        if (colorAttachmentSpec.TextureFormat == PixelFormat::None)
        {
            NX_ASSERT(0, "Pixel format cannot be PixelFormat::None for a color attachment");
        }

        Nexus::Graphics::Texture2DSpecification spec;
        spec.Width = m_Specification.Width;
        spec.Height = m_Specification.Height;
        spec.Format = colorAttachmentSpec.TextureFormat;
        spec.Samples = m_Specification.Samples;
        spec.Usage = {TextureUsage::Sampled, TextureUsage::RenderTarget};
        auto texture = std::dynamic_pointer_cast<Texture2D_Vk>(m_Device->CreateTexture2D(spec));
        m_ColorAttachments.push_back(texture);
    }
}

void FramebufferVk::CreateDepthTargets()
{
    // the specification does not contain a depth attachment, so we do not create one
    if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
    {
        Nexus::Graphics::Texture2DSpecification spec;
        spec.Width = m_Specification.Width;
        spec.Height = m_Specification.Height;
        spec.Format = m_Specification.DepthAttachmentSpecification.DepthFormat;
        spec.Samples = m_Specification.Samples;
        spec.Usage = {TextureUsage::DepthStencil, TextureUsage::RenderTarget};
        m_DepthAttachment = std::dynamic_pointer_cast<Texture2D_Vk>(m_Device->CreateTexture2D(spec));
    }
}
} // namespace Nexus::Graphics

#endif