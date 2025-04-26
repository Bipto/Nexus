#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"
	#include "ResourceSetVk.hpp"

namespace Nexus::Graphics
{
	class GraphicsPipelineVk : public GraphicsPipeline
	{
	  public:
		GraphicsPipelineVk(const GraphicsPipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		~GraphicsPipelineVk();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;
		VkPipeline						   GetPipeline();
		VkPipelineLayout				   GetPipelineLayout();

	  private:
		VkPipelineShaderStageCreateInfo		   CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
		VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology);
		VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullingFlags);
		VkPipelineMultisampleStateCreateInfo   CreateMultisampleStateCreateInfo();
		std::vector<VkPipelineColorBlendAttachmentState> CreateColorBlendAttachmentStates();
		VkPipelineLayoutCreateInfo			   CreatePipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &layouts);
		VkPipelineDepthStencilStateCreateInfo  CreatePipelineDepthStencilStateCreateInfo();

		VkPrimitiveTopology GetPrimitiveTopology();
		VkPolygonMode		GetPolygonMode();
		VkCullModeFlags		GetCullMode();

		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages();

	  private:
		VkPipelineLayout  m_PipelineLayout;
		VkPipeline		  m_Pipeline;
		GraphicsDeviceVk *m_GraphicsDevice;
	};
}	 // namespace Nexus::Graphics

#endif