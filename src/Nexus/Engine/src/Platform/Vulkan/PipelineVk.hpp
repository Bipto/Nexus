#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "RHI/Pipeline.hpp"
	#include "ResourceSetVk.hpp"

namespace Nexus::Graphics
{
	class PipelineVk
	{
	  public:
		virtual ~PipelineVk()
		{
		}

		virtual void Bind(VkCommandBuffer cmd, VkRenderPass renderPass)								= 0;
		virtual void SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc) = 0;

		const std::map<uint32_t, VkDescriptorSetLayout> &GetDescriptorSetLayouts() const
		{
			return m_DescriptorSetLayouts;
		}

		const std::map<VkDescriptorType, uint32_t> &GetDescriptorCounts() const
		{
			return m_DescriptorCounts;
		}

		VkPipelineLayout GetPipelineLayout() const
		{
			return m_PipelineLayout;
		}

	  protected:
		std::map<uint32_t, VkDescriptorSetLayout> m_DescriptorSetLayouts = {};
		std::map<VkDescriptorType, uint32_t>	  m_DescriptorCounts	 = {};
		VkPipelineLayout						  m_PipelineLayout		 = VK_NULL_HANDLE;
	};

	class GraphicsPipelineVk : public IGraphicsPipeline, public PipelineVk
	{
	  public:
		GraphicsPipelineVk(const GraphicsPipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		virtual ~GraphicsPipelineVk();
		virtual const GraphicsPipelineDescription &GetPipelineDescription() const override;

		void Bind(VkCommandBuffer cmd, VkRenderPass renderPass) final;
		void SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc) final;

	  private:
		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages();

	  private:
		std::map<VkRenderPass, VkPipeline> m_Pipelines		= {};
		GraphicsDeviceVk				  *m_GraphicsDevice = {};
	};

	class MeshletPipelineVk : public IMeshletPipeline, public PipelineVk
	{
	  public:
		MeshletPipelineVk(const MeshletPipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		virtual ~MeshletPipelineVk();

		void Bind(VkCommandBuffer cmd, VkRenderPass renderPass) final;
		void SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc) final;

	  private:
		std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages();

	  private:
		std::map<VkRenderPass, VkPipeline> m_Pipelines		= {};
		GraphicsDeviceVk				  *m_GraphicsDevice = {};
	};

	class ComputePipelineVk : public IComputePipeline, public PipelineVk
	{
	  public:
		ComputePipelineVk(const ComputePipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		virtual ~ComputePipelineVk();
		void Bind(VkCommandBuffer cmd, VkRenderPass renderPass) final;
		void SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc) final;

	  private:
		VkPipeline		  m_Pipeline	   = {};
		GraphicsDeviceVk *m_GraphicsDevice = {};
	};

	class RayTracingPipelineVk : public IRayTracingPipeline, public PipelineVk
	{
	  public:
		RayTracingPipelineVk(const RayTracingPipelineDescription &description, GraphicsDeviceVk *graphicsDevice);
		virtual ~RayTracingPipelineVk();
		void				 Bind(VkCommandBuffer cmd, VkRenderPass renderPass) final;
		void				 SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc) final;
		std::vector<uint8_t> GetRayTracingShaderGroupHandles() const final;

	  private:
		VkPipeline		  m_Pipeline	   = {};
		GraphicsDeviceVk *m_GraphicsDevice = {};
	};
}	 // namespace Nexus::Graphics

#endif