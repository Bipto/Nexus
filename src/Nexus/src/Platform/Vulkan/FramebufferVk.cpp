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
    vkDestroyFramebuffer(m_Device->GetVkDevice(), m_Framebuffer, nullptr);
    vkDestroyRenderPass(m_Device->GetVkDevice(), m_FramebufferRenderPass, nullptr);
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

VkFramebuffer FramebufferVk::GetVkFramebuffer()
{
    return m_Framebuffer;
}

Ref<Texture> FramebufferVk::GetColorTexture(uint32_t index)
{
    return m_ColorAttachments.at(index);
}

Ref<Texture> FramebufferVk::GetDepthTexture()
{
    return m_DepthAttachment;
}

Ref<TextureVk> FramebufferVk::GetVulkanColorTexture(uint32_t index)
{
    return m_ColorAttachments.at(index);
}

Ref<TextureVk> FramebufferVk::GetVulkanDepthTexture()
{
    return m_DepthAttachment;
}

VkRenderPass FramebufferVk::GetRenderPass()
{
    return m_FramebufferRenderPass;
}

void FramebufferVk::Recreate()
{
    CreateColorTargets();
    CreateDepthTargets();
    CreateRenderPass();
    CreateFramebuffer();
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

        Nexus::Graphics::TextureSpecification spec;
        spec.Width = m_Specification.Width;
        spec.Height = m_Specification.Height;
        spec.Format = colorAttachmentSpec.TextureFormat;
        spec.Samples = m_Specification.Samples;
        spec.Usage = {TextureUsage::Sampled, TextureUsage::RenderTarget};
        auto texture = std::dynamic_pointer_cast<TextureVk>(m_Device->CreateTexture(spec));
        m_ColorAttachments.push_back(texture);
    }
}

void FramebufferVk::CreateDepthTargets()
{
    // the specification does not contain a depth attachment, so we do not create one
    if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
    {
        Nexus::Graphics::TextureSpecification spec;
        spec.Width = m_Specification.Width;
        spec.Height = m_Specification.Height;
        spec.Format = m_Specification.DepthAttachmentSpecification.DepthFormat;
        spec.Samples = m_Specification.Samples;
        spec.Usage = {TextureUsage::DepthStencil, TextureUsage::RenderTarget};
        m_DepthAttachment = std::dynamic_pointer_cast<TextureVk>(m_Device->CreateTexture(spec));
    }
}

void FramebufferVk::CreateFramebuffer()
{
    std::vector<VkImageView> attachments;
    for (const auto texture : m_ColorAttachments)
    {
        attachments.push_back(texture->GetImageView());
    }

    if (HasDepthTexture())
    {
        attachments.push_back(m_DepthAttachment->GetImageView());
    }

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_FramebufferRenderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = m_Specification.Width;
    framebufferInfo.height = m_Specification.Height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_Device->GetVkDevice(), &framebufferInfo, nullptr, &m_Framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create framebuffer");
    }
}

void FramebufferVk::CreateRenderPass()
{
    std::vector<VkAttachmentDescription> colorAttachmentDescriptions;
    std::vector<VkAttachmentReference> colorAttachmentReferences;
    std::vector<VkSubpassDependency> subpassDependencies;
    std::vector<VkAttachmentDescription> subpassAttachments;

    uint32_t attachmentIndex = 0;
    VkSampleCountFlagBits samples = Vk::GetVkSampleCount(m_Specification.Samples);

    for (const auto &colorAttachment : m_Specification.ColorAttachmentSpecification.Attachments)
    {
        VkAttachmentDescription attachment = {};
        attachment.format = Vk::GetVkPixelDataFormat(colorAttachment.TextureFormat, false);
        attachment.samples = samples;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        colorAttachmentDescriptions.push_back(attachment);
        subpassAttachments.push_back(attachment);

        VkAttachmentReference colorReference = {};
        colorReference.attachment = attachmentIndex;
        colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachmentReferences.push_back(colorReference);

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        subpassDependencies.push_back(dependency);

        attachmentIndex++;
    }

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = colorAttachmentReferences.size();
    subpass.pColorAttachments = colorAttachmentReferences.data();

    VkAttachmentReference depthReference = {};

    // create depth information for render pass
    if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
    {
        VkAttachmentDescription depthAttachment = {};
        depthAttachment.flags = 0;
        depthAttachment.format = Vk::GetVkPixelDataFormat(m_Specification.DepthAttachmentSpecification.DepthFormat, true);
        depthAttachment.samples = samples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subpassAttachments.push_back(depthAttachment);

        depthReference.attachment = attachmentIndex;
        depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subpass.pDepthStencilAttachment = &depthReference;

        VkSubpassDependency depthDependency;
        depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depthDependency.dstSubpass = 0;
        depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.srcAccessMask = 0;
        depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        depthDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        subpassDependencies.push_back(depthDependency);
    }

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = subpassAttachments.size();
    renderPassInfo.pAttachments = subpassAttachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = subpassDependencies.size();
    renderPassInfo.pDependencies = subpassDependencies.data();

    if (vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassInfo, nullptr, &m_FramebufferRenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass");
    }
}
} // namespace Nexus::Graphics

#endif