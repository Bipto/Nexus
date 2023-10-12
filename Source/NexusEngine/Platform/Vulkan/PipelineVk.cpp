#if defined(NX_PLATFORM_VULKAN)

#include "PipelineVk.hpp"
#include "ShaderVk.hpp"
#include "RenderPassVk.hpp"
#include "ResourceSetVk.hpp"

namespace Nexus::Graphics
{
    VkCompareOp GetCompareOp(ComparisonFunction function)
    {
        switch (function)
        {
        case ComparisonFunction::Always:
            return VK_COMPARE_OP_ALWAYS;
        case ComparisonFunction::Equal:
            return VK_COMPARE_OP_EQUAL;
        case ComparisonFunction::Greater:
            return VK_COMPARE_OP_GREATER;
        case ComparisonFunction::GreaterEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case ComparisonFunction::Less:
            return VK_COMPARE_OP_LESS;
        case ComparisonFunction::LessEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case ComparisonFunction::Never:
            return VK_COMPARE_OP_NEVER;
        case ComparisonFunction::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        }
    }

    PipelineVk::PipelineVk(const PipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
        : Pipeline(description), m_GraphicsDevice(graphicsDevice)
    {
        auto vulkanShader = (ShaderVk *)description.Shader;
        auto vulkanRenderPass = (RenderPassVk *)description.RenderPass;

        auto resourceSet = new ResourceSetVk(description.ResourceSetSpecification, graphicsDevice);
        std::vector<VkDescriptorSetLayout> layouts;

        if (resourceSet->HasUniformBuffers())
        {
            layouts.push_back(resourceSet->GetUniformBufferDescriptorSetLayout());
        }

        if (resourceSet->HasTextures())
        {
            layouts.push_back(resourceSet->GetSamplerDescriptorSetLayout());
        }

        // describes data that will be uploaded to the shader
        VkPipelineLayoutCreateInfo layoutInfo = CreatePipelineLayoutCreateInfo(layouts);
        if (vkCreatePipelineLayout(graphicsDevice->GetVkDevice(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout");
        }

        delete resourceSet;

        auto bindingDescription = GetBindingDescription();
        auto attributeDescriptions = GetAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = CreateVertexInputStateCreateInfo(bindingDescription, attributeDescriptions);
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = CreateInputAssemblyCreateInfo(GetPrimitiveTopology());

        VkViewport viewport;
        viewport.x = description.Viewport.X;
        viewport.y = description.Viewport.Height + description.Viewport.Y;
        viewport.width = description.Viewport.Width;
        viewport.height = -description.Viewport.Height;
        viewport.minDepth = description.Viewport.MinDepth;
        viewport.maxDepth = description.Viewport.MaxDepth;

        VkRect2D scissor;
        scissor.offset = {
            description.RasterizerStateDescription.ScissorRectangle.X,
            description.RasterizerStateDescription.ScissorRectangle.Y};
        scissor.extent = {
            (uint32_t)description.RasterizerStateDescription.ScissorRectangle.Width,
            (uint32_t)description.RasterizerStateDescription.ScissorRectangle.Height};

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo = CreatePipelineDepthStencilStateCreateInfo();
        VkPipelineRasterizationStateCreateInfo rasterizerInfo = CreateRasterizationStateCreateInfo(GetPolygonMode(), GetCullMode());
        VkPipelineMultisampleStateCreateInfo multisampleInfo = CreateMultisampleStateCreateInfo();

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;

        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        std::vector<VkPipelineColorBlendAttachmentState> blendStates;
        for (int i = 0; i <= vulkanRenderPass->GetColorAttachmentCount(); i++)
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
        pipelineInfo.renderPass = vulkanRenderPass->GetVkRenderPass();
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

    VkPipelineVertexInputStateCreateInfo PipelineVk::CreateVertexInputStateCreateInfo(VkVertexInputBindingDescription bindingDescription, const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
    {
        VkPipelineVertexInputStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.vertexBindingDescriptionCount = 1;
        info.pVertexBindingDescriptions = &bindingDescription;

        uint32_t attributeCount = static_cast<uint32_t>(attributeDescriptions.size());
        info.vertexAttributeDescriptionCount = attributeCount;
        info.pVertexAttributeDescriptions = attributeDescriptions.data();

        return info;
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
        VkPipelineMultisampleStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.sampleShadingEnable = VK_FALSE;
        info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
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
        info.minDepthBounds = 0.0f;
        info.maxDepthBounds = 1.0f;

        info.stencilTestEnable = m_Description.DepthStencilDescription.EnableStencilTest;

        return info;
    }

    VkVertexInputBindingDescription PipelineVk::GetBindingDescription()
    {
        auto shader = m_Description.Shader;
        auto layout = shader->GetLayout();

        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = layout.GetStride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> PipelineVk::GetAttributeDescriptions()
    {
        const auto &layout = m_Description.Shader->GetLayout();

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        uint32_t index = 0;

        for (const auto &element : layout)
        {
            VkVertexInputAttributeDescription description;
            description.binding = 0;
            description.location = index;
            description.format = GetShaderDataType(element.Type);
            description.offset = element.Offset;
            attributeDescriptions.push_back(description);
            index++;
        }

        return attributeDescriptions;
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