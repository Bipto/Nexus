#if defined(NX_PLATFORM_VULKAN)

	#include "PipelineVk.hpp"

	#include "FramebufferVk.hpp"
	#include "ResourceSetVk.hpp"
	#include "ShaderModuleVk.hpp"

namespace Nexus::Graphics
{
	GraphicsPipelineVk::GraphicsPipelineVk(const GraphicsPipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: GraphicsPipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		m_PipelineLayout = Vk::CreatePipelineLayout(m_Description.ResourceSetSpec, graphicsDevice);

		std::string debugName = description.DebugName + " - Pipeline Layout";
		graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)m_PipelineLayout, debugName.c_str());
	}

	GraphicsPipelineVk::~GraphicsPipelineVk()
	{
		for (const auto &[renderPass, pipeline] : m_Pipelines) { vkDestroyPipeline(m_GraphicsDevice->GetVkDevice(), pipeline, nullptr); }

		vkDestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);
	}

	const GraphicsPipelineDescription &GraphicsPipelineVk::GetPipelineDescription() const
	{
		return m_Description;
	}

	VkPipelineLayout GraphicsPipelineVk::GetPipelineLayout()
	{
		return m_PipelineLayout;
	}

	void GraphicsPipelineVk::Bind(VkCommandBuffer cmd, VkRenderPass renderPass)
	{
		if (m_Pipelines.find(renderPass) == m_Pipelines.end())
		{
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages = GetShaderStages();
			m_Pipelines[renderPass]									  = Vk::CreateGraphicsPipeline(renderPass,
																   m_GraphicsDevice,
																   m_Description.DepthStencilDesc,
																   m_Description.RasterizerStateDesc,
																   m_Description.ColourTargetSampleCount,
																   shaderStages,
																   m_Description.ColourTargetCount,
																   m_Description.ColourFormats,
																   m_Description.ColourBlendStates,
																   m_Description.DepthFormat,
																   m_PipelineLayout,
																   m_Description.PrimitiveTopology,
																   m_Description.Layouts);

			m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Pipelines[renderPass], m_Description.DebugName.c_str());
		}

		VkPipeline pipeline = m_Pipelines.at(renderPass);
		NX_ASSERT(pipeline, "Failed to find a valid pipeline");
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void GraphicsPipelineVk::SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet)
	{
		const auto &descriptorSets = resourceSet->GetDescriptorSets()[m_GraphicsDevice->GetCurrentFrameIndex()];
		for (const auto &set : descriptorSets)
		{
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, set.first, 1, &set.second, 0, nullptr);
		}
	}

	std::vector<VkPipelineShaderStageCreateInfo> GraphicsPipelineVk::GetShaderStages()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		if (m_Description.FragmentModule)
		{
			auto vulkanFragmentModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.FragmentModule);
			NX_ASSERT(vulkanFragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanFragmentModule));
		}

		if (m_Description.GeometryModule)
		{
			auto vulkanGeometryModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.GeometryModule);
			NX_ASSERT(vulkanGeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanGeometryModule));
		}

		if (m_Description.TesselationControlModule)
		{
			auto vulkanTesselationControlModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.TesselationControlModule);
			NX_ASSERT(vulkanTesselationControlModule->GetShaderStage() == ShaderStage::TesselationControl,
					  "Shader module is not a tesselation control shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanTesselationControlModule));
		}

		if (m_Description.TesselationEvaluationModule)
		{
			auto vulkanTesselationEvaluation = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.TesselationEvaluationModule);
			NX_ASSERT(vulkanTesselationEvaluation->GetShaderStage() == ShaderStage::TesselationEvaluation,
					  "Shader module is not a tesselation evaluation shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanTesselationEvaluation));
		}

		if (m_Description.VertexModule)
		{
			auto vulkanVertexModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.VertexModule);
			NX_ASSERT(vulkanVertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanVertexModule));
		}

		return shaderStages;
	}

	MeshletPipelineVk::MeshletPipelineVk(const MeshletPipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: MeshletPipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		m_PipelineLayout = Vk::CreatePipelineLayout(m_Description.ResourceSetSpec, graphicsDevice);

		std::string debugName = description.DebugName + " - Pipeline Layout";
		graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)m_PipelineLayout, debugName.c_str());
	}

	MeshletPipelineVk::~MeshletPipelineVk()
	{
		for (const auto &[renderPass, pipeline] : m_Pipelines) { vkDestroyPipeline(m_GraphicsDevice->GetVkDevice(), pipeline, nullptr); }

		vkDestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);
	}

	VkPipelineLayout MeshletPipelineVk::GetPipelineLayout()
	{
		return m_PipelineLayout;
	}

	void MeshletPipelineVk::Bind(VkCommandBuffer cmd, VkRenderPass renderPass)
	{
		if (m_Pipelines.find(renderPass) == m_Pipelines.end())
		{
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages = GetShaderStages();
			m_Pipelines[renderPass]									  = Vk::CreateGraphicsPipeline(renderPass,
																   m_GraphicsDevice,
																   m_Description.DepthStencilDesc,
																   m_Description.RasterizerStateDesc,
																   m_Description.ColourTargetSampleCount,
																   shaderStages,
																   m_Description.ColourTargetCount,
																   m_Description.ColourFormats,
																   m_Description.ColourBlendStates,
																   m_Description.DepthFormat,
																   m_PipelineLayout,
																   m_Description.PrimitiveTopology,
																								   {});

			m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Pipelines[renderPass], m_Description.DebugName.c_str());
		}

		VkPipeline pipeline = m_Pipelines.at(renderPass);
		NX_ASSERT(pipeline, "Failed to find a valid pipeline");
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void MeshletPipelineVk::SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet)
	{
		const auto &descriptorSets = resourceSet->GetDescriptorSets()[m_GraphicsDevice->GetCurrentFrameIndex()];
		for (const auto &set : descriptorSets)
		{
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, set.first, 1, &set.second, 0, nullptr);
		}
	}

	std::vector<VkPipelineShaderStageCreateInfo> MeshletPipelineVk::GetShaderStages()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		if (m_Description.FragmentModule)
		{
			auto vulkanFragmentModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.FragmentModule);
			NX_ASSERT(vulkanFragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanFragmentModule));
		}

		if (m_Description.MeshModule)
		{
			auto vulkanMeshModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.MeshModule);
			NX_ASSERT(vulkanMeshModule->GetShaderStage() == ShaderStage::Mesh, "Shader module is not a mesh shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanMeshModule));
		}

		if (m_Description.TaskModule)
		{
			auto vulkanTaskModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.MeshModule);
			NX_ASSERT(vulkanTaskModule->GetShaderStage() == ShaderStage::Task, "Shader module is not a task shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanTaskModule));
		}

		return shaderStages;
	}

	ComputePipelineVk::ComputePipelineVk(const ComputePipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: ComputePipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		NX_ASSERT(description.ComputeShader->GetShaderStage() == ShaderStage::Compute,
				  "Shader passed to ComputePipelineDescription was not a compute shader");

		// pipeline layout
		{
			m_PipelineLayout	  = Vk::CreatePipelineLayout(m_Description.ResourceSetSpec, graphicsDevice);
			std::string debugName = description.DebugName + " - Pipeline Layout";
			graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)m_PipelineLayout, debugName.c_str());
		}

		Ref<ShaderModuleVk> shaderModule = std::dynamic_pointer_cast<ShaderModuleVk>(description.ComputeShader);

		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		shaderStageInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.stage							= VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageInfo.module							= shaderModule->GetShaderModule();
		shaderStageInfo.pName							= "main";

		VkComputePipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType						 = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.stage						 = shaderStageInfo;
		pipelineInfo.layout						 = m_PipelineLayout;

		if (vkCreateComputePipelines(m_GraphicsDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create compute pipeline");
		}

		graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Pipeline, m_Description.DebugName.c_str());
	}

	ComputePipelineVk::~ComputePipelineVk()
	{
	}

	void ComputePipelineVk::Bind(VkCommandBuffer cmd, VkRenderPass renderPass)
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
	}

	void ComputePipelineVk::SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet)
	{
		const auto &descriptorSets = resourceSet->GetDescriptorSets()[m_GraphicsDevice->GetCurrentFrameIndex()];
		for (const auto &set : descriptorSets)
		{
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_PipelineLayout, set.first, 1, &set.second, 0, nullptr);
		}
	}
}	 // namespace Nexus::Graphics

#endif