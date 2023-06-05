#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class PipelineBuilder
{
public:
    std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
    VkPipelineVertexInputStateCreateInfo VertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo InputAssembly;
    VkViewport Viewport;
    VkRect2D Scissor;
    VkPipelineRasterizationStateCreateInfo Rasterizer;
    VkPipelineColorBlendAttachmentState ColorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo Multisampling;
    VkPipelineLayout PipelineLayout;
    VkPipelineDepthStencilStateCreateInfo DepthStencil;

public:
    VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
    VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo(VkVertexInputBindingDescription bindingDescription, const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions);
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology);
    VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygonMode);
    VkPipelineMultisampleStateCreateInfo MultisamplingStateCreateInfo();
    VkPipelineColorBlendAttachmentState ColorBlendAttachmentState();
    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &layouts);
    VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo();
    VkPipeline BuildPipeline(VkDevice device, VkRenderPass pass);
};