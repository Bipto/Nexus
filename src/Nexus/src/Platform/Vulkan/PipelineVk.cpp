#if defined(NX_PLATFORM_VULKAN)

	#include "PipelineVk.hpp"

	#include "FramebufferVk.hpp"
	#include "ResourceSetVk.hpp"
	#include "ShaderModuleVk.hpp"

namespace Nexus::Graphics
{
	GraphicsPipelineVk::GraphicsPipelineVk(const GraphicsPipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: IGraphicsPipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		m_PipelineLayout = Vk::CreatePipelineLayout(this, graphicsDevice, m_DescriptorSetLayouts, m_DescriptorCounts);

		std::string debugName = description.DebugName + " - Pipeline Layout";
		graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)m_PipelineLayout, debugName.c_str());
	}

	GraphicsPipelineVk::~GraphicsPipelineVk()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		for (const auto &[renderPass, pipeline] : m_Pipelines) { context.DestroyPipeline(m_GraphicsDevice->GetVkDevice(), pipeline, nullptr); }

		context.DestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);

		for (const auto &[setIndex, descriptorSetLayout] : m_DescriptorSetLayouts)
		{
			context.DestroyDescriptorSetLayout(m_GraphicsDevice->GetVkDevice(), descriptorSetLayout, nullptr);
		}
	}

	const GraphicsPipelineDescription &GraphicsPipelineVk::GetPipelineDescription() const
	{
		return m_Description;
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
																   m_Description.Samples,
																   shaderStages,
																   m_Description.ColourTargetCount,
																   m_Description.ColourFormats,
																   m_Description.ColourBlendStates,
																   m_Description.DepthFormat,
																   m_PipelineLayout,
																   m_Description.PrimitiveTopology,
																   m_Description.Layouts,
																   &m_Description.SampleMask);

			m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Pipelines[renderPass], m_Description.DebugName.c_str());
		}

		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		VkPipeline pipeline = m_Pipelines.at(renderPass);
		NX_VALIDATE(pipeline, "Failed to find a valid pipeline");
		context.CmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void GraphicsPipelineVk::SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc)
	{
		const GladVulkanContext &context	 = m_GraphicsDevice->GetVulkanContext();
		Ref<ResourceSetVk>		 resourceSet = std::dynamic_pointer_cast<ResourceSetVk>(desc.TargetResourceSet);
		if (resourceSet)
		{
			resourceSet->Bind(context, cmd, this, VK_PIPELINE_BIND_POINT_GRAPHICS, desc.DynamicOffsets);
		}
	}

	std::vector<VkPipelineShaderStageCreateInfo> GraphicsPipelineVk::GetShaderStages()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		if (m_Description.FragmentModule)
		{
			auto vulkanFragmentModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.FragmentModule);
			NX_VALIDATE(vulkanFragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanFragmentModule));
		}

		if (m_Description.GeometryModule)
		{
			auto vulkanGeometryModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.GeometryModule);
			NX_VALIDATE(vulkanGeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanGeometryModule));
		}

		if (m_Description.TesselationControlModule)
		{
			auto vulkanTesselationControlModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.TesselationControlModule);
			NX_VALIDATE(vulkanTesselationControlModule->GetShaderStage() == ShaderStage::TessellationControl,
						"Shader module is not a tesselation control shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanTesselationControlModule));
		}

		if (m_Description.TesselationEvaluationModule)
		{
			auto vulkanTesselationEvaluation = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.TesselationEvaluationModule);
			NX_VALIDATE(vulkanTesselationEvaluation->GetShaderStage() == ShaderStage::TessellationEvaluation,
						"Shader module is not a tesselation evaluation shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanTesselationEvaluation));
		}

		if (m_Description.VertexModule)
		{
			auto vulkanVertexModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.VertexModule);
			NX_VALIDATE(vulkanVertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanVertexModule));
		}

		return shaderStages;
	}

	MeshletPipelineVk::MeshletPipelineVk(const MeshletPipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: IMeshletPipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		m_PipelineLayout = Vk::CreatePipelineLayout(this, graphicsDevice, m_DescriptorSetLayouts, m_DescriptorCounts);

		std::string debugName = description.DebugName + " - Pipeline Layout";
		graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)m_PipelineLayout, debugName.c_str());
	}

	MeshletPipelineVk::~MeshletPipelineVk()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		for (const auto &[renderPass, pipeline] : m_Pipelines) { context.DestroyPipeline(m_GraphicsDevice->GetVkDevice(), pipeline, nullptr); }

		context.DestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);

		for (const auto &[setIndex, descriptorSetLayout] : m_DescriptorSetLayouts)
		{
			context.DestroyDescriptorSetLayout(m_GraphicsDevice->GetVkDevice(), descriptorSetLayout, nullptr);
		}
	}

	void MeshletPipelineVk::Bind(VkCommandBuffer cmd, VkRenderPass renderPass)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		if (m_Pipelines.find(renderPass) == m_Pipelines.end())
		{
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages = GetShaderStages();
			m_Pipelines[renderPass]									  = Vk::CreateGraphicsPipeline(renderPass,
																   m_GraphicsDevice,
																   m_Description.DepthStencilDesc,
																   m_Description.RasterizerStateDesc,
																   m_Description.Samples,
																   shaderStages,
																   m_Description.ColourTargetCount,
																   m_Description.ColourFormats,
																   m_Description.ColourBlendStates,
																   m_Description.DepthFormat,
																   m_PipelineLayout,
																   m_Description.PrimitiveTopology,
																								   {},
																   &m_Description.SampleMask);

			m_GraphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Pipelines[renderPass], m_Description.DebugName.c_str());
		}

		VkPipeline pipeline = m_Pipelines.at(renderPass);
		NX_VALIDATE(pipeline, "Failed to find a valid pipeline");
		context.CmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void MeshletPipelineVk::SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc)
	{
		const GladVulkanContext &context	 = m_GraphicsDevice->GetVulkanContext();
		Ref<ResourceSetVk>		 resourceSet = std::dynamic_pointer_cast<ResourceSetVk>(desc.TargetResourceSet);

		if (resourceSet)
		{
			const auto &descriptorSets = resourceSet->GetDescriptorSets();

			std::vector<uint32_t> offsets = {};

			for (const auto &set : descriptorSets)
			{
				context.CmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, set.first, 1, &set.second, 0, nullptr);
			}
		}
	}

	std::vector<VkPipelineShaderStageCreateInfo> MeshletPipelineVk::GetShaderStages()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		if (m_Description.FragmentModule)
		{
			auto vulkanFragmentModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.FragmentModule);
			NX_VALIDATE(vulkanFragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanFragmentModule));
		}

		if (m_Description.MeshModule)
		{
			auto vulkanMeshModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.MeshModule);
			NX_VALIDATE(vulkanMeshModule->GetShaderStage() == ShaderStage::Mesh, "Shader module is not a mesh shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanMeshModule));
		}

		if (m_Description.TaskModule)
		{
			auto vulkanTaskModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.MeshModule);
			NX_VALIDATE(vulkanTaskModule->GetShaderStage() == ShaderStage::Task, "Shader module is not a task shader");
			shaderStages.push_back(Vk::CreateShaderStageCreateInfo(vulkanTaskModule));
		}

		return shaderStages;
	}

	ComputePipelineVk::ComputePipelineVk(const ComputePipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: IComputePipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		NX_VALIDATE(description.ComputeShader->GetShaderStage() == ShaderStage::Compute,
					"Shader passed to ComputePipelineDescription was not a compute shader");

		// pipeline layout
		{
			m_PipelineLayout	  = Vk::CreatePipelineLayout(this, graphicsDevice, m_DescriptorSetLayouts, m_DescriptorCounts);
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

		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		if (context.CreateComputePipelines(m_GraphicsDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create compute pipeline");
		}

		graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Pipeline, m_Description.DebugName.c_str());
	}

	ComputePipelineVk::~ComputePipelineVk()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		context.DestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);

		for (const auto &[setIndex, descriptorSetLayout] : m_DescriptorSetLayouts)
		{
			context.DestroyDescriptorSetLayout(m_GraphicsDevice->GetVkDevice(), descriptorSetLayout, nullptr);
		}

		context.DestroyPipeline(m_GraphicsDevice->GetVkDevice(), m_Pipeline, nullptr);
	}

	void ComputePipelineVk::Bind(VkCommandBuffer cmd, VkRenderPass renderPass)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
		context.CmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
	}

	void ComputePipelineVk::SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc)
	{
		const GladVulkanContext &context	 = m_GraphicsDevice->GetVulkanContext();
		Ref<ResourceSetVk>		 resourceSet = std::dynamic_pointer_cast<ResourceSetVk>(desc.TargetResourceSet);

		if (resourceSet)
		{
			resourceSet->Bind(context, cmd, this, VK_PIPELINE_BIND_POINT_GRAPHICS, desc.DynamicOffsets);
		}
	}

	RayTracingPipelineVk::RayTracingPipelineVk(const RayTracingPipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: IRayTracingPipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		// pipeline layout
		{
			m_PipelineLayout	  = Vk::CreatePipelineLayout(this, graphicsDevice, m_DescriptorSetLayouts, m_DescriptorCounts);
			std::string debugName = description.DebugName + " - Pipeline Layout";
			graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)m_PipelineLayout, debugName.c_str());
		}

		// pipeline
		{
			std::vector<VkPipelineShaderStageCreateInfo>	  shaderStages = {};
			std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups = {};

			for (const Ref<IShaderModule> &shaderModule : description.Shaders)
			{
				Ref<ShaderModuleVk> vulkanShader = std::dynamic_pointer_cast<ShaderModuleVk>(shaderModule);

				VkPipelineShaderStageCreateInfo &shaderInfo = shaderStages.emplace_back();
				shaderInfo									= Vk::CreateShaderStageCreateInfo(vulkanShader);
			}

			for (const ShaderGroup &shaderGroup : description.ShaderGroups)
			{
				VkRayTracingShaderGroupCreateInfoKHR &vkShaderGroup = shaderGroups.emplace_back();
				vkShaderGroup.sType									= VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
				vkShaderGroup.pNext									= nullptr;
				vkShaderGroup.type									= Vk::GetRayTracingShaderGroupType(shaderGroup.Type);
				vkShaderGroup.generalShader							= shaderGroup.GeneralShader;
				vkShaderGroup.closestHitShader						= shaderGroup.ClosestHitShader;
				vkShaderGroup.anyHitShader							= shaderGroup.AnyHitShader;
				vkShaderGroup.intersectionShader					= shaderGroup.IntersectionShader;
				vkShaderGroup.pShaderGroupCaptureReplayHandle		= nullptr;
			}

			VkRayTracingPipelineCreateInfoKHR pipelineInfo = {};
			pipelineInfo.sType							   = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
			pipelineInfo.pNext							   = nullptr;
			pipelineInfo.flags							   = 0;
			pipelineInfo.stageCount						   = shaderStages.size();
			pipelineInfo.pStages						   = shaderStages.data();
			pipelineInfo.groupCount						   = shaderGroups.size();
			pipelineInfo.pGroups						   = shaderGroups.data();
			pipelineInfo.maxPipelineRayRecursionDepth	   = description.MaxRecursionDepth;
			pipelineInfo.pLibraryInfo					   = nullptr;
			pipelineInfo.pLibraryInterface				   = nullptr;
			pipelineInfo.pDynamicState					   = nullptr;
			pipelineInfo.layout							   = m_PipelineLayout;
			pipelineInfo.basePipelineHandle				   = VK_NULL_HANDLE;
			pipelineInfo.basePipelineIndex				   = 0;

			const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
			if (context.CreateRayTracingPipelinesKHR != nullptr)
			{
				NX_VALIDATE(context.CreateRayTracingPipelinesKHR(m_GraphicsDevice->GetVkDevice(),
																 VK_NULL_HANDLE,
																 VK_NULL_HANDLE,
																 1,
																 &pipelineInfo,
																 nullptr,
																 &m_Pipeline) == VK_SUCCESS,
							"Failed to create ray tracing pipeline");
			}

			graphicsDevice->SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Pipeline, m_Description.DebugName.c_str());
		}
	}

	RayTracingPipelineVk::~RayTracingPipelineVk()
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		context.DestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);

		for (const auto &[setIndex, descriptorSetLayout] : m_DescriptorSetLayouts)
		{
			context.DestroyDescriptorSetLayout(m_GraphicsDevice->GetVkDevice(), descriptorSetLayout, nullptr);
		}

		context.DestroyPipeline(m_GraphicsDevice->GetVkDevice(), m_Pipeline, nullptr);
	}

	void RayTracingPipelineVk::Bind(VkCommandBuffer cmd, VkRenderPass renderPass)
	{
		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();
		context.CmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_Pipeline);
	}

	void RayTracingPipelineVk::SetResourceSet(VkCommandBuffer cmd, const ResourceSetBindingDescription &desc)
	{
		const GladVulkanContext &context	 = m_GraphicsDevice->GetVulkanContext();
		Ref<ResourceSetVk>		 resourceSet = std::dynamic_pointer_cast<ResourceSetVk>(desc.TargetResourceSet);

		if (resourceSet)
		{
			resourceSet->Bind(context, cmd, this, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, desc.DynamicOffsets);
		}
	}

	std::vector<uint8_t> RayTracingPipelineVk::GetRayTracingShaderGroupHandles() const
	{
		RayTracingDeviceDescription rayTracingDeviceDesc = m_GraphicsDevice->GetRayTracingDeviceDescription();
		std::vector<uint8_t>		handles(m_Description.ShaderGroups.size() * rayTracingDeviceDesc.ShaderGroupHandleSize);

		const GladVulkanContext &context = m_GraphicsDevice->GetVulkanContext();

		if (context.GetRayTracingShaderGroupHandlesKHR)
		{
			VkResult result = context.GetRayTracingShaderGroupHandlesKHR(m_GraphicsDevice->GetVkDevice(),
																		 m_Pipeline,
																		 0,
																		 m_Description.ShaderGroups.size(),
																		 handles.size(),
																		 handles.data());
			NX_VALIDATE(result == VK_SUCCESS, "Failed to retrieve shader group handles");
		}

		return handles;
	}
}	 // namespace Nexus::Graphics

#endif