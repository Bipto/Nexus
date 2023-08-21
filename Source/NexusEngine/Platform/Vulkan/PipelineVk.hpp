#pragma once

#include "Core/Graphics/Pipeline.hpp"
#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    class PipelineVk : public Pipeline
    {
    public:
        PipelineVk(const PipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
        ~PipelineVk();
        virtual const PipelineDescription &GetPipelineDescription() const override;

    private:
        VkPipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
        VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo(VkVertexInputBindingDescription bindingDescription, const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions);
        VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology);
        VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullingFlags);
        VkPipelineMultisampleStateCreateInfo CreateMultisampleStateCreateInfo();
        VkPipelineColorBlendAttachmentState CreateColorBlendAttachmentState();
        VkPipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &layouts);
        VkPipelineDepthStencilStateCreateInfo CreatePipelineDepthStencilStateCreateInfo();
        VkPipeline BuildPipeline();

        VkVertexInputBindingDescription GetBindingDescription();
        std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        VkPrimitiveTopology GetPrimitiveTopology();
        VkPolygonMode GetPolygonMode();
        VkCullModeFlags GetCullMode();

    private:
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;
        GraphicsDeviceVk *m_GraphicsDevice;
    };
}