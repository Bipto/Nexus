#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/Pipeline.hpp"
	#include "ResourceSetVk.hpp"

namespace Nexus::Graphics
{
	class PipelineVk
	{
	  public:
		virtual ~PipelineVk()
		{
		}

		virtual void Bind(VkCommandBuffer cmd)											 = 0;
		virtual void SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet) = 0;
	};

	VkPipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &layouts);

	class GraphicsPipelineVk : public GraphicsPipeline, public PipelineVk
	{
	  public:
		GraphicsPipelineVk(const GraphicsPipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		~GraphicsPipelineVk();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;
		VkPipeline								   GetPipeline();
		VkPipelineLayout						   GetPipelineLayout();

		virtual void Bind(VkCommandBuffer cmd) final;
		virtual void SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet) final;

	  private:
		VkPipelineShaderStageCreateInfo					 CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
		VkPipelineInputAssemblyStateCreateInfo			 CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology);
		VkPipelineRasterizationStateCreateInfo			 CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullingFlags);
		VkPipelineMultisampleStateCreateInfo			 CreateMultisampleStateCreateInfo();
		std::vector<VkPipelineColorBlendAttachmentState> CreateColorBlendAttachmentStates();
		VkPipelineDepthStencilStateCreateInfo			 CreatePipelineDepthStencilStateCreateInfo();

		VkPrimitiveTopology GetPrimitiveTopology();
		VkPolygonMode		GetPolygonMode();
		VkCullModeFlags		GetCullMode();

		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages();
		void										 CreateRenderPass();

	  private:
		VkPipelineLayout  m_PipelineLayout;
		VkPipeline		  m_Pipeline;
		GraphicsDeviceVk *m_GraphicsDevice;
		VkRenderPass	  m_RenderPass = VK_NULL_HANDLE;
	};

	class ComputePipelineVk : public ComputePipeline, public PipelineVk
	{
	  public:
		ComputePipelineVk(const ComputePipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		virtual ~ComputePipelineVk();
		virtual void Bind(VkCommandBuffer cmd) final;
		virtual void SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet) final;

	  private:
		VkPipelineLayout  m_PipelineLayout;
		VkPipeline		  m_Pipeline;
		GraphicsDeviceVk *m_GraphicsDevice;
	};
}	 // namespace Nexus::Graphics

#endif