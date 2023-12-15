#if defined(NX_PLATFORM_VULKAN)

#include "RenderPassVk.hpp"

namespace Nexus::Graphics
{
    VkAttachmentLoadOp GetVkLoadOpFromNexusLoadOp(LoadOperation op)
    {
        switch (op)
        {
        case LoadOperation::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
            break;
        case LoadOperation::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
            break;
        default:
            throw std::runtime_error("Failed to select a valid LoadOperation");
        }
    }

    RenderPassVk::RenderPassVk(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification, GraphicsDeviceVk *graphicsDevice)
        : RenderPass(renderPassSpecification, framebufferSpecification)
    {
        m_GraphicsDevice = graphicsDevice;
        SetupForFramebuffer();
        m_ColorAttachmentCount = framebufferSpecification.ColorAttachmentSpecification.Attachments.size();
    }

    RenderPassVk::RenderPassVk(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain, GraphicsDeviceVk *graphicsDevice)
        : RenderPass(renderPassSpecification, swapchain)
    {
        m_GraphicsDevice = graphicsDevice;
        SetupForSwapchain();
        m_ColorAttachmentCount = 1;
    }

    RenderPassVk::~RenderPassVk()
    {
        vkDestroyRenderPass(m_GraphicsDevice->GetVkDevice(), m_RenderPass, nullptr);
    }

    VkRenderPass RenderPassVk::GetVkRenderPass()
    {
        return m_RenderPass;
    }

    uint32_t RenderPassVk::GetColorAttachmentCount()
    {
        return 0;
    }

    void RenderPassVk::SetupForFramebuffer()
    {
        const auto &framebufferSpecification = GetData<FramebufferSpecification>();
        std::vector<VkAttachmentDescription> colorAttachmentDescriptions;
        std::vector<VkAttachmentReference> colorAttachmentReferences;
        std::vector<VkSubpassDependency> subpassDependencies;
        std::vector<VkAttachmentDescription> subpassAttachments;
        int attachmentIndex = 0;

        for (const auto &colorAttachment : framebufferSpecification.ColorAttachmentSpecification.Attachments)
        {
            VkAttachmentDescription attachment = {};
            attachment.format = GetVkTextureFormatFromNexusFormat(colorAttachment.TextureFormat);
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // GetVkLoadOpFromNexusLoadOp(m_RenderPassSpecification.ColorLoadOperation);
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // GetVkLoadOpFromNexusLoadOp(m_RenderPassSpecification.StencilDepthLoadOperation);
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

            if (GetColorLoadOperation() == LoadOperation::Clear)
            {
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            else
            {
                attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

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

        if (framebufferSpecification.DepthAttachmentSpecification.DepthFormat != DepthFormat::None)
        {
            VkAttachmentDescription depthAttachment = {};
            depthAttachment.flags = 0;
            depthAttachment.format = GetVkDepthFormatFromNexusFormat(framebufferSpecification.DepthAttachmentSpecification.DepthFormat);
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // GetVkLoadOpFromNexusLoadOp(m_RenderPassSpecification.StencilDepthLoadOperation);
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // GetVkLoadOpFromNexusLoadOp(m_RenderPassSpecification.StencilDepthLoadOperation);
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

            if (GetDepthStencilLoadOperation() == LoadOperation::Clear)
            {
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            else
            {
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }

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

        if (vkCreateRenderPass(m_GraphicsDevice->GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }

    void RenderPassVk::SetupForSwapchain()
    {
        SwapchainVk *swapchain = (SwapchainVk *)m_GraphicsDevice->GetPrimaryWindow()->GetSwapchain();
        VkDevice device = m_GraphicsDevice->GetVkDevice();

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = swapchain->m_SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        if (GetColorLoadOperation() == LoadOperation::Clear)
        {
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        else
        {
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment = {};
        depthAttachment.flags = 0;
        depthAttachment.format = swapchain->m_DepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

        if (GetDepthStencilLoadOperation() == LoadOperation::Clear)
        {
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        else
        {
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentReference = {};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;
        subpass.pDepthStencilAttachment = &depthAttachmentReference;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkSubpassDependency depthDependency;
        depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depthDependency.dstSubpass = 0;
        depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.srcAccessMask = 0;
        depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        depthDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkSubpassDependency dependencies[2] = {dependency, depthDependency};
        VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.pAttachments = &attachments[0];
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = &dependencies[0];

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }
}

#endif