#if defined(NX_PLATFORM_VULKAN)

#include "FramebufferVk.hpp"

namespace Nexus::Graphics
{
    FramebufferVk::FramebufferVk(const FramebufferSpecification &spec, GraphicsDeviceVk *device)
        : Framebuffer(spec), m_Device(device)
    {
        m_Specification = spec;
        Recreate();
    }

    FramebufferVk::~FramebufferVk()
    {
        for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
        {
            vkDestroyImage(m_Device->GetVkDevice(), m_Images[i], nullptr);
            vkFreeMemory(m_Device->GetVkDevice(), m_ImageMemory[i], nullptr);
            vkDestroySampler(m_Device->GetVkDevice(), m_Samplers[i], nullptr);
            vkDestroyImageView(m_Device->GetVkDevice(), m_ImageViews[i], nullptr);
        }

        if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
        {
            vkDestroyImage(m_Device->GetVkDevice(), m_DepthImage, nullptr);
            vkFreeMemory(m_Device->GetVkDevice(), m_DepthMemory, nullptr);
            vkDestroyImageView(m_Device->GetVkDevice(), m_DepthImageView, nullptr);
        }

        vkDestroyFramebuffer(m_Device->GetVkDevice(), m_Framebuffer, nullptr);
        vkDestroyRenderPass(m_Device->GetVkDevice(), m_FramebufferRenderPass, nullptr);
    }

    void *FramebufferVk::GetColorAttachment(int index)
    {
        return m_ImageViews[index];
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

    void *FramebufferVk::GetDepthAttachment()
    {
        return m_DepthImageView;
    }

    VkFramebuffer FramebufferVk::GetVkFramebuffer()
    {
        return m_Framebuffer;
    }

    VkImage FramebufferVk::GetColorTextureImage(uint32_t index)
    {
        return m_Images[index];
    }

    VkImageView FramebufferVk::GetColorTextureImageView(uint32_t index)
    {
        return m_ImageViews[index];
    }

    VkSampler FramebufferVk::GetColorTextureSampler(uint32_t index)
    {
        return m_Samplers[index];
    }

    VkImage FramebufferVk::GetDepthTextureImage()
    {
        return m_DepthImage;
    }

    VkRenderPass FramebufferVk::GetRenderPass()
    {
        return m_FramebufferRenderPass;
    }

    const std::vector<VkImageLayout> &FramebufferVk::GetColorImageLayouts()
    {
        return m_ImageLayouts;
    }

    VkImageLayout FramebufferVk::GetDepthImageLayout()
    {
        return m_DepthLayout;
    }

    void FramebufferVk::SetColorImageLayout(VkImageLayout layout, uint32_t index)
    {
        if (index > m_Images.size())
        {
            return;
        }

        m_ImageLayouts[index] = layout;
    }

    void FramebufferVk::SetDepthImageLayout(VkImageLayout layout)
    {
        m_DepthLayout = layout;
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
        m_Images.resize(m_Specification.ColorAttachmentSpecification.Attachments.size());
        m_ImageMemory.resize(m_Specification.ColorAttachmentSpecification.Attachments.size());
        m_Samplers.resize(m_Specification.ColorAttachmentSpecification.Attachments.size());
        m_ImageViews.resize(m_Specification.ColorAttachmentSpecification.Attachments.size());
        m_ImageLayouts.clear();

        VkSampleCountFlagBits sampleCount = GetVkSampleCount(m_Specification.Samples);

        for (int i = 0; i < m_Images.size(); i++)
        {
            VkFormat format = GetVkPixelDataFormat(m_Specification.ColorAttachmentSpecification.Attachments[i].TextureFormat, false);

            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.format = format;
            imageInfo.extent.width = m_Specification.Width;
            imageInfo.extent.height = m_Specification.Height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = sampleCount;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            VkMemoryAllocateInfo memAlloc{};
            memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

            if (vkCreateImage(m_Device->GetVkDevice(), &imageInfo, nullptr, &m_Images[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image");
            }

            VkMemoryRequirements memRequirements{};
            vkGetImageMemoryRequirements(m_Device->GetVkDevice(), m_Images[i], &memRequirements);
            memAlloc.allocationSize = memRequirements.size;
            memAlloc.memoryTypeIndex = m_Device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            if (vkAllocateMemory(m_Device->GetVkDevice(), &memAlloc, nullptr, &m_ImageMemory[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image memory");
            }

            if (vkBindImageMemory(m_Device->GetVkDevice(), m_Images[i], m_ImageMemory[i], 0) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to bind image memory");
            }

            VkSamplerCreateInfo samplerInfo = {};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.maxAnisotropy = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 1.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            if (vkCreateSampler(m_Device->GetVkDevice(), &samplerInfo, nullptr, &m_Samplers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create sampler");
            }

            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_Images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = format;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device->GetVkDevice(), &viewInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create texture image view");
            }

            m_ImageLayouts.push_back(VK_IMAGE_LAYOUT_UNDEFINED);
        }
    }

    void FramebufferVk::CreateDepthTargets()
    {
        // the specification does not contain a depth attachment, so we do not create one
        if (m_Specification.DepthAttachmentSpecification.DepthFormat == PixelFormat::None)
            return;

        VkFormat depthFormat = GetVkPixelDataFormat(m_Specification.DepthAttachmentSpecification.DepthFormat, true);
        VkSampleCountFlagBits sampleCount = GetVkSampleCount(m_Specification.Samples);

        // allocate image
        {
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = m_Specification.Width;
            imageInfo.extent.height = m_Specification.Height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            imageInfo.samples = sampleCount;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateImage(m_Device->GetVkDevice(), &imageInfo, nullptr, &m_DepthImage) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(m_Device->GetVkDevice(), m_DepthImage, &memRequirements);

            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = m_Device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            if (vkAllocateMemory(m_Device->GetVkDevice(), &allocInfo, nullptr, &m_DepthMemory) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to alloate image memory");
            }

            vkBindImageMemory(m_Device->GetVkDevice(), m_DepthImage, m_DepthMemory, 0);
        }

        // allocate image view
        {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_DepthImage;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device->GetVkDevice(), &viewInfo, nullptr, &m_DepthImageView) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image view");
            }
        }

        m_DepthLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    void FramebufferVk::CreateFramebuffer()
    {
        std::vector<VkImageView> attachments;
        for (const auto imageView : m_ImageViews)
        {
            attachments.push_back(imageView);
        }
        if (this->HasDepthTexture())
        {
            attachments.push_back(m_DepthImageView);
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
        VkSampleCountFlagBits samples = GetVkSampleCount(m_Specification.Samples);

        for (const auto &colorAttachment : m_Specification.ColorAttachmentSpecification.Attachments)
        {
            VkAttachmentDescription attachment = {};
            attachment.format = GetVkPixelDataFormat(colorAttachment.TextureFormat, false);
            attachment.samples = samples;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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

        // create depth information for render pass
        if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
        {
            VkAttachmentDescription depthAttachment = {};
            depthAttachment.flags = 0;
            depthAttachment.format = GetVkPixelDataFormat(m_Specification.DepthAttachmentSpecification.DepthFormat, true);
            depthAttachment.samples = samples;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            subpassAttachments.push_back(depthAttachment);

            VkAttachmentReference depthReference = {};
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
}

#endif