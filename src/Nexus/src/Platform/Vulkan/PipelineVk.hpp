#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"
	#include "ResourceSetVk.hpp"

namespace Nexus::Graphics
{
	class PipelineVk : public Pipeline
	{
	  public:
		PipelineVk(const PipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		~PipelineVk();
		virtual const PipelineDescription &GetPipelineDescription() const override;
		VkPipeline						   GetPipeline();
		VkPipelineLayout				   GetPipelineLayout();

	  private:
		VkPipelineShaderStageCreateInfo		   CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
		VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology);
		VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullingFlags);
		VkPipelineMultisampleStateCreateInfo   CreateMultisampleStateCreateInfo();
		VkPipelineColorBlendAttachmentState	   CreateColorBlendAttachmentState();
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