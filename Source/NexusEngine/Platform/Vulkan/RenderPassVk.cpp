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
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_GraphicsDevice = graphicsDevice;
        m_Data = framebufferSpecification;
        m_DataType = RenderPassDataType::Framebuffer;
        SetupForFramebuffer();
        m_ColorAttachmentCount = framebufferSpecification.ColorAttachmentSpecification.Attachments.size();
    }

    RenderPassVk::RenderPassVk(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain, GraphicsDeviceVk *graphicsDevice)
    {
        m_RenderPassSpecification = renderPassSpecification;
        m_GraphicsDevice = graphicsDevice;
        m_Data = swapchain;
        m_DataType = RenderPassDataType::Swapchain;
        SetupForSwapchain();
        m_ColorAttachmentCount = 1;
    }

    RenderPassVk::~RenderPassVk()
    {
    }

    LoadOperation RenderPassVk::GetColorLoadOperation()
    {
        return m_RenderPassSpecification.ColorLoadOperation;
    }

    LoadOperation RenderPassVk::GetDepthStencilLoadOperation()
    {
        return m_RenderPassSpecification.StencilDepthLoadOperation;
    }

    const RenderPassSpecification &RenderPassVk::GetRenderPassSpecification()
    {
        return m_RenderPassSpecification;
    }

    const RenderPassData &RenderPassVk::GetRenderPassData()
    {
        return m_Data;
    }

    RenderPassDataType RenderPassVk::GetRenderPassDataType()
    {
        return m_DataType;
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
        /* const auto &framebufferSpecification = spec.Framebuffer->GetFramebufferSpecification();
        std::vector<VkAttachmentDescription> attachments;
        for (int i = 0; i < framebufferSpecification.ColorAttachmentSpecification.Attachments.size(); i++)
        {
            const FramebufferTextureSpecification &colorAttachmentSpec = framebufferSpecification.ColorAttachmentSpecification.Attachments[i];

            // creating attachments
            {
                VkAttachmentDescription attachment;
                attachment.format = GetVkFormatFromNexusFormat(colorAttachmentSpec.TextureFormat);
                attachment.samples = VK_SAMPLE_COUNT_1_BIT;
                attachment.loadOp = GetVkLoadOpFromNexusLoadOp(spec.ColorLoadOperation);
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachment.stencilLoadOp = GetVkLoadOpFromNexusLoadOp(spec.StencilDepthLoadOperation);
                attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
        } */

        throw std::runtime_error("Function is not implemented yet");
    }

    void RenderPassVk::SetupForSwapchain()
    {
        SwapchainVk *swapchain = (SwapchainVk *)m_GraphicsDevice->GetSwapchain();
        VkDevice device = m_GraphicsDevice->GetVkDevice();

        std::vector<VkAttachmentDescription> attachments(2);

        attachments[0].format = swapchain->m_SurfaceFormat.format;
        attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachments[1].format = swapchain->m_DepthFormat;
        attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorReference = {};
        colorReference.attachment = 0;
        colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthReference = {};
        depthReference.attachment = 1;
        depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorReference;
        subpassDescription.pDepthStencilAttachment = &depthReference;
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pInputAttachments = nullptr;
        subpassDescription.preserveAttachmentCount = 0;
        subpassDescription.pPreserveAttachments = nullptr;
        subpassDescription.pResolveAttachments = nullptr;

        std::vector<VkSubpassDependency> dependencies(1);
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescription;
        renderPassInfo.dependencyCount = (uint32_t)dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass");
        }
    }
}