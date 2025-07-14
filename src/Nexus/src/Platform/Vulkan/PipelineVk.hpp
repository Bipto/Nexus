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

		virtual void Bind(VkCommandBuffer cmd, VkRenderPass renderPass) = 0;
		virtual void SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet)						   = 0;
	};

	class GraphicsPipelineVk : public GraphicsPipeline, public PipelineVk
	{
	  public:
		GraphicsPipelineVk(const GraphicsPipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		~GraphicsPipelineVk();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;
		VkPipelineLayout						   GetPipelineLayout();

		virtual void Bind(VkCommandBuffer cmd, VkRenderPass renderPass) final;
		virtual void SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet) final;

	  private:
		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages();

	  private:
		VkPipelineLayout				   m_PipelineLayout;
		std::map<VkRenderPass, VkPipeline> m_Pipelines;
		GraphicsDeviceVk				  *m_GraphicsDevice;
	};

	class MeshletPipelineVk : public MeshletPipeline, public PipelineVk
	{
	};

	class ComputePipelineVk : public ComputePipeline, public PipelineVk
	{
	  public:
		ComputePipelineVk(const ComputePipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		virtual ~ComputePipelineVk();
		virtual void Bind(VkCommandBuffer cmd, VkRenderPass renderPass) final;
		virtual void SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet) final;

	  private:
		VkPipelineLayout  m_PipelineLayout;
		VkPipeline		  m_Pipeline;
		GraphicsDeviceVk *m_GraphicsDevice;
	};
}	 // namespace Nexus::Graphics

#endif