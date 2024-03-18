#if defined(NX_PLATFORM_VULKAN)

#include "PipelineVk.hpp"
#include "ShaderVk.hpp"
#include "ResourceSetVk.hpp"
#include "FramebufferVk.hpp"

namespace Nexus::Graphics
{
    PipelineVk::PipelineVk(const PipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
        : Pipeline(description), m_GraphicsDevice(graphicsDevice)
    {
        auto vulkanShader = std::dynamic_pointer_cast<ShaderVk>(description.Shader);
        auto resourceSet = new ResourceSetVk(description.ResourceSetSpecification, graphicsDevice);

        const auto &pipelineLayouts = resourceSet->GetDescriptorSetLayouts();
        std::vector<VkDescriptorSetLayout> layouts;
        for (const auto &layout : pipelineLayouts)
        {
            layouts.push_back(layout.second);
        }

        VkPipelineLayoutCreateInfo layoutInfo = CreatePipelineLayoutCreateInfo(layouts);
        if (vkCreatePipelineLayout(graphicsDevice->GetVkDevice(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout");
        }

        delete resourceSet;

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo = CreatePipelineDepthStencilStateCreateInfo();
        VkPipelineRasterizationStateCreateInfo rasterizerInfo = CreateRasterizationStateCreateInfo(GetPolygonMode(), GetCullMode());
        VkPipelineMultisampleStateCreateInfo multisampleInfo = CreateMultisampleStateCreateInfo();

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;

        viewportState.viewportCount = 1;
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 1;
        viewportState.pScissors = nullptr;

        std::vector<VkPipelineColorBlendAttachmentState> blendStates;
        for (int i = 0; i < m_Description.Target.GetColorAttachmentCount(); i++)
        {
            blendStates.push_back(CreateColorBlendAttachmentState());
        }

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = blendStates.size();
        colorBlending.pAttachments = blendStates.data();

        const auto &shaderStages = vulkanShader->GetShaderStages();

        // create vertex input layout
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;

        uint32_t elementIndex = 0;
        for (uint32_t layoutIndex = 0; layoutIndex < m_Description.Layouts.size(); layoutIndex++)
        {
            const auto &layout = m_Description.Layouts.at(layoutIndex);

            // create attribute descriptions
            for (uint32_t i = 0; i < layout.GetNumberOfElements(); i++)
            {
                const auto &element = layout.GetElement(i);

                VkVertexInputAttributeDescription attributeDescription = {};
                attributeDescription.binding = layoutIndex;
                attributeDescription.location = elementIndex;
                attributeDescription.format = GetShaderDataType(element.Type);
                attributeDescription.offset = element.Offset;
                attributeDescriptions.push_back(attributeDescription);
                elementIndex++;
            }

            // create binding descriptions
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = layoutIndex;
            bindingDescription.stride = layout.GetStride();
            bindingDescription.inputRate = (layout.GetInstanceStepRate() != 0) ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescriptions.push_back(bindingDescription);
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = CreateInputAssemblyCreateInfo(GetPrimitiveTopology());

        // create pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizerInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.layout = m_PipelineLayout;

        VkPipelineDynamicStateCreateInfo dynamicInfo = {};
        dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicInfo.pNext = nullptr;
        dynamicInfo.flags = 0;

        std::vector<VkDynamicState> dynamicStates =
            {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR};

        dynamicInfo.dynamicStateCount = dynamicStates.size();
        dynamicInfo.pDynamicStates = dynamicStates.data();

        pipelineInfo.pDynamicState = &dynamicInfo;

        if (m_Description.Target.GetType() == RenderTargetType::Swapchain)
        {
            auto swapchainVk = (SwapchainVk *)m_Description.Target.GetData<Swapchain *>();
            pipelineInfo.renderPass = swapchainVk->GetRenderPass();
        }
        else if (m_Description.Target.GetType() == RenderTargetType::Framebuffer)
        {
            auto framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(m_Description.Target.GetData<Ref<Framebuffer>>());
            pipelineInfo.renderPass = framebufferVk->GetRenderPass();
        }
        else
        {
            throw std::runtime_error("Invalid render target entered");
        }

        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(m_GraphicsDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline");
        }
    }

    PipelineVk::~PipelineVk()
    {
        vkDestroyPipeline(m_GraphicsDevice->GetVkDevice(), m_Pipeline, nullptr);
        vkDestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);
    }

    const PipelineDescription &PipelineVk::GetPipelineDescription() const
    {
        return m_Description;
    }

    VkPipeline PipelineVk::GetPipeline()
    {
        return m_Pipeline;
    }

    VkPipelineLayout PipelineVk::GetPipelineLayout()
    {
        return m_PipelineLayout;
    }

    VkPipelineShaderStageCreateInfo PipelineVk::CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module)
    {
        VkPipelineShaderStageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.stage = stage;
        createInfo.module = module;
        createInfo.pName = "main";
        return createInfo;
    }

    VkPipelineInputAssemblyStateCreateInfo PipelineVk::CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology)
    {
        VkPipelineInputAssemblyStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.topology = topology;
        info.primitiveRestartEnable = VK_FALSE;
        return info;
    }

    VkPipelineRasterizationStateCreateInfo PipelineVk::CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullingFlags)
    {
        VkPipelineRasterizationStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.depthClampEnable = VK_FALSE;
        info.rasterizerDiscardEnable = VK_FALSE;

        info.polygonMode = polygonMode;
        info.lineWidth = 1.0f;
        info.cullMode = cullingFlags;
        info.depthBiasEnable = VK_FALSE;
        info.depthBiasConstantFactor = 0.0f;
        info.depthBiasClamp = 0.0f;
        info.depthBiasSlopeFactor = 0.0f;

        return info;
    }

    VkPipelineMultisampleStateCreateInfo PipelineVk::CreateMultisampleStateCreateInfo()
    {
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        // multisampling is only supported on framebuffers
        if (m_Description.Target.GetType() == RenderTargetType::Framebuffer)
        {
            auto framebufferVk = std::dynamic_pointer_cast<FramebufferVk>(m_Description.Target.GetData<Ref<Framebuffer>>());
            samples = GetVkSampleCount(framebufferVk->GetFramebufferSpecification().Samples);
        }
        else if (m_Description.Target.GetType() == RenderTargetType::Swapchain)
        {
            auto swapchainVk = (SwapchainVk *)m_Description.Target.GetData<Swapchain *>();
            samples = GetVkSampleCount(swapchainVk->GetSpecification().Samples);
        }
        else
        {
            throw std::runtime_error("Failed to find a valid render target type");
        }

        VkPipelineMultisampleStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.sampleShadingEnable = VK_FALSE;
        info.minSampleShading = 0.0f;
        info.rasterizationSamples = samples;
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;
        return info;
    }

    VkPipelineColorBlendAttachmentState PipelineVk::CreateColorBlendAttachmentState()
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = m_Description.BlendStateDescription.EnableBlending;
        colorBlendAttachment.srcColorBlendFactor = GetVkBlendFactor(m_Description.BlendStateDescription.SourceColourBlend);
        colorBlendAttachment.dstColorBlendFactor = GetVkBlendFactor(m_Description.BlendStateDescription.DestinationColourBlend);
        colorBlendAttachment.colorBlendOp = GetVkBlendOp(m_Description.BlendStateDescription.BlendEquation);
        colorBlendAttachment.srcAlphaBlendFactor = GetVkBlendFactor(m_Description.BlendStateDescription.SourceAlphaBlend);
        colorBlendAttachment.dstAlphaBlendFactor = GetVkBlendFactor(m_Description.BlendStateDescription.DestinationAlphaBlend);
        colorBlendAttachment.alphaBlendOp = GetVkBlendOp(m_Description.BlendStateDescription.BlendEquation);
        return colorBlendAttachment;
    }

    VkPipelineLayoutCreateInfo PipelineVk::CreatePipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &layouts)
    {
        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.setLayoutCount = layouts.size();
        info.pSetLayouts = layouts.data();
        info.pushConstantRangeCount = 0;
        info.pPushConstantRanges = nullptr;
        return info;
    }

    VkPipelineDepthStencilStateCreateInfo PipelineVk::CreatePipelineDepthStencilStateCreateInfo()
    {
        VkPipelineDepthStencilStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.depthTestEnable = m_Description.DepthStencilDescription.EnableDepthTest;
        info.depthWriteEnable = m_Description.DepthStencilDescription.EnableDepthWrite;
        info.depthCompareOp = GetCompareOp(m_Description.DepthStencilDescription.DepthComparisonFunction);

        info.depthBoundsTestEnable = VK_FALSE;
        info.minDepthBounds = m_Description.DepthStencilDescription.MinDepth;
        info.maxDepthBounds = m_Description.DepthStencilDescription.MaxDepth;

        info.stencilTestEnable = m_Description.DepthStencilDescription.EnableStencilTest;

        return info;
    }

    VkPrimitiveTopology PipelineVk::GetPrimitiveTopology()
    {
        switch (m_Description.PrimitiveTopology)
        {
        case Nexus::Graphics::Topology::LineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case Nexus::Graphics::Topology::LineStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        case Nexus::Graphics::Topology::PointList:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        case Nexus::Graphics::Topology::TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case Nexus::Graphics::Topology::TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        default:
            throw std::runtime_error("An invalid primitive topology was entered");
            break;
        }

        return VkPrimitiveTopology();
    }

    VkPolygonMode PipelineVk::GetPolygonMode()
    {
        switch (m_Description.RasterizerStateDescription.FillMode)
        {
        case Nexus::Graphics::FillMode::Solid:
            return VK_POLYGON_MODE_FILL;
            break;
        case Nexus::Graphics::FillMode::Wireframe:
            return VK_POLYGON_MODE_LINE;
            break;
        default:
            throw std::runtime_error("An invalid fill mode was entered");
            break;
        }
    }

    VkCullModeFlags PipelineVk::GetCullMode()
    {
        switch (m_Description.RasterizerStateDescription.CullMode)
        {
        case Nexus::Graphics::CullMode::None:
            return VK_CULL_MODE_NONE;
            break;
        case Nexus::Graphics::CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
            break;
        case Nexus::Graphics::CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
            break;
        default:
            throw std::runtime_error("An invalid culling mode was entered");
            break;
        }
    }
}

#endif