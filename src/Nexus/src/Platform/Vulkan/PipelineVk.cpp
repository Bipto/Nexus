#if defined(NX_PLATFORM_VULKAN)

	#include "PipelineVk.hpp"

	#include "FramebufferVk.hpp"
	#include "ResourceSetVk.hpp"
	#include "ShaderModuleVk.hpp"

namespace Nexus::Graphics
{
	VkPipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &layouts)
	{
		VkPipelineLayoutCreateInfo info = {};
		info.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext						= nullptr;
		info.flags						= 0;
		info.setLayoutCount				= layouts.size();
		info.pSetLayouts				= layouts.data();
		info.pushConstantRangeCount		= 0;
		info.pPushConstantRanges		= nullptr;
		return info;
	}

	GraphicsPipelineVk::GraphicsPipelineVk(const GraphicsPipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: GraphicsPipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		std::unique_ptr<ResourceSetVk> resourceSet = std::make_unique<ResourceSetVk>(description.ResourceSetSpec, graphicsDevice);

		const auto						  &pipelineLayouts = resourceSet->GetDescriptorSetLayouts();
		std::vector<VkDescriptorSetLayout> layouts;
		for (const auto &layout : pipelineLayouts) { layouts.push_back(layout.second); }

		VkPipelineLayoutCreateInfo layoutInfo = CreatePipelineLayoutCreateInfo(layouts);
		if (vkCreatePipelineLayout(graphicsDevice->GetVkDevice(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}

		VkPipelineDepthStencilStateCreateInfo  depthStencilInfo = CreatePipelineDepthStencilStateCreateInfo();
		VkPipelineRasterizationStateCreateInfo rasterizerInfo	= CreateRasterizationStateCreateInfo(GetPolygonMode(), GetCullMode());
		VkPipelineMultisampleStateCreateInfo   multisampleInfo	= CreateMultisampleStateCreateInfo();

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType								= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext								= nullptr;

		viewportState.viewportCount = 1;
		viewportState.pViewports	= nullptr;
		viewportState.scissorCount	= 1;
		viewportState.pScissors		= nullptr;

		std::vector<VkPipelineColorBlendAttachmentState> blendStates = CreateColorBlendAttachmentStates();

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType								  = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext								  = nullptr;
		colorBlending.logicOpEnable						  = VK_FALSE;
		colorBlending.logicOp							  = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount					  = blendStates.size();
		colorBlending.pAttachments						  = blendStates.data();

		// create vertex input layout
		std::vector<VkVertexInputAttributeDescription>		   attributeDescriptions;
		std::vector<VkVertexInputBindingDescription>		   bindingDescriptions;
		std::vector<VkVertexInputBindingDivisorDescriptionEXT> divisorDescriptions;

		uint32_t elementIndex = 0;
		for (uint32_t layoutIndex = 0; layoutIndex < m_Description.Layouts.size(); layoutIndex++)
		{
			const auto &layout = m_Description.Layouts.at(layoutIndex);

			// create attribute descriptions
			for (uint32_t i = 0; i < layout.GetNumberOfElements(); i++)
			{
				const auto &element = layout.GetElement(i);

				VkVertexInputAttributeDescription attributeDescription = {};
				attributeDescription.binding						   = layoutIndex;
				attributeDescription.location						   = elementIndex;
				attributeDescription.format							   = Vk::GetShaderDataType(element.Type);
				attributeDescription.offset							   = element.Offset;
				attributeDescriptions.push_back(attributeDescription);

				elementIndex++;
			}

			// create binding descriptions
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding						   = layoutIndex;
			bindingDescription.stride						   = 0;
			bindingDescription.inputRate = (layout.GetInstanceStepRate() != 0) ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescriptions.push_back(bindingDescription);

			if (layout.IsInstanceBuffer())
			{
				VkVertexInputBindingDivisorDescriptionEXT divisorDescription = {};
				divisorDescription.binding									 = layoutIndex;
				divisorDescription.divisor									 = layout.GetInstanceStepRate();
				divisorDescriptions.push_back(divisorDescription);
			}
		}

		VkPipelineVertexInputDivisorStateCreateInfoEXT divisorInfo = {};
		divisorInfo.sType										   = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT;
		divisorInfo.pNext										   = nullptr;
		divisorInfo.vertexBindingDivisorCount					   = divisorDescriptions.size();
		divisorInfo.pVertexBindingDivisors						   = divisorDescriptions.data();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType								 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.pNext								 = nullptr;

		if (divisorInfo.vertexBindingDivisorCount > 0)
		{
			vertexInputInfo.pNext = &divisorInfo;
		}

		vertexInputInfo.vertexBindingDescriptionCount	= bindingDescriptions.size();
		vertexInputInfo.pVertexBindingDescriptions		= bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions	= attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = CreateInputAssemblyCreateInfo(GetPrimitiveTopology());

		const auto &shaderStages = GetShaderStages();

		std::vector<VkFormat> colourFormats;

		for (uint32_t i = 0; i < m_Description.ColourTargetCount; i++)
		{
			colourFormats.push_back(Vk::GetVkPixelDataFormat(m_Description.ColourFormats[i], false));
		}

		VkFormat depthStencilFormat = Vk::GetVkPixelDataFormat(m_Description.DepthFormat, true);

		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo = {};
		pipelineRenderingCreateInfo.sType						  = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCreateInfo.colorAttachmentCount		  = colourFormats.size();
		pipelineRenderingCreateInfo.pColorAttachmentFormats		  = colourFormats.data();
		pipelineRenderingCreateInfo.depthAttachmentFormat		  = depthStencilFormat;
		pipelineRenderingCreateInfo.stencilAttachmentFormat		  = depthStencilFormat;

		// create pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType						  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext						  = &pipelineRenderingCreateInfo;
		pipelineInfo.stageCount					  = shaderStages.size();
		pipelineInfo.pStages					  = shaderStages.data();
		pipelineInfo.pVertexInputState			  = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState		  = &inputAssemblyInfo;
		pipelineInfo.pViewportState				  = &viewportState;
		pipelineInfo.pRasterizationState		  = &rasterizerInfo;
		pipelineInfo.pMultisampleState			  = &multisampleInfo;
		pipelineInfo.pColorBlendState			  = &colorBlending;
		pipelineInfo.pDepthStencilState			  = &depthStencilInfo;
		pipelineInfo.layout						  = m_PipelineLayout;

		VkPipelineDynamicStateCreateInfo dynamicInfo = {};
		dynamicInfo.sType							 = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicInfo.pNext							 = nullptr;
		dynamicInfo.flags							 = 0;

		std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
													 VK_DYNAMIC_STATE_SCISSOR,
													 VK_DYNAMIC_STATE_STENCIL_REFERENCE,
													 VK_DYNAMIC_STATE_DEPTH_BOUNDS,
													 VK_DYNAMIC_STATE_BLEND_CONSTANTS,
													 VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE};

		dynamicInfo.dynamicStateCount = dynamicStates.size();
		dynamicInfo.pDynamicStates	  = dynamicStates.data();

		pipelineInfo.pDynamicState = &dynamicInfo;
		pipelineInfo.renderPass	   = nullptr;

		pipelineInfo.subpass			= 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_GraphicsDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}
	}

	GraphicsPipelineVk::~GraphicsPipelineVk()
	{
		vkDestroyPipeline(m_GraphicsDevice->GetVkDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_GraphicsDevice->GetVkDevice(), m_PipelineLayout, nullptr);
	}

	const GraphicsPipelineDescription &GraphicsPipelineVk::GetPipelineDescription() const
	{
		return m_Description;
	}

	VkPipeline GraphicsPipelineVk::GetPipeline()
	{
		return m_Pipeline;
	}

	VkPipelineLayout GraphicsPipelineVk::GetPipelineLayout()
	{
		return m_PipelineLayout;
	}

	void GraphicsPipelineVk::Bind(VkCommandBuffer cmd)
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	}

	void GraphicsPipelineVk::SetResourceSet(VkCommandBuffer cmd, Ref<ResourceSetVk> resourceSet)
	{
		const auto &descriptorSets = resourceSet->GetDescriptorSets()[m_GraphicsDevice->GetCurrentFrameIndex()];
		for (const auto &set : descriptorSets)
		{
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, set.first, 1, &set.second, 0, nullptr);
		}
	}

	VkPipelineShaderStageCreateInfo GraphicsPipelineVk::CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module)
	{
		VkPipelineShaderStageCreateInfo createInfo = {};
		createInfo.sType						   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.pNext						   = nullptr;
		createInfo.stage						   = stage;
		createInfo.module						   = module;
		createInfo.pName						   = "main";
		return createInfo;
	}

	VkPipelineInputAssemblyStateCreateInfo GraphicsPipelineVk::CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology)
	{
		VkPipelineInputAssemblyStateCreateInfo info = {};
		info.sType									= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		info.pNext									= nullptr;
		info.topology								= topology;
		info.primitiveRestartEnable					= VK_FALSE;
		return info;
	}

	VkPipelineRasterizationStateCreateInfo GraphicsPipelineVk::CreateRasterizationStateCreateInfo(VkPolygonMode	  polygonMode,
																								  VkCullModeFlags cullingFlags)
	{
		VkPipelineRasterizationStateCreateInfo info = {};
		info.sType									= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		info.pNext									= nullptr;
		info.depthClampEnable						= VK_FALSE;
		info.rasterizerDiscardEnable				= VK_FALSE;

		info.polygonMode			 = polygonMode;
		info.lineWidth				 = 1.0f;
		info.cullMode				 = cullingFlags;
		info.depthBiasEnable		 = VK_FALSE;
		info.depthBiasConstantFactor = 0.0f;
		info.depthBiasClamp			 = 0.0f;
		info.depthBiasSlopeFactor	 = 0.0f;

		info.frontFace = Vk::GetFrontFace(m_Description.RasterizerStateDesc.TriangleFrontFace);

		return info;
	}

	VkPipelineMultisampleStateCreateInfo GraphicsPipelineVk::CreateMultisampleStateCreateInfo()
	{
		VkSampleCountFlagBits samples = Vk::GetVkSampleCountFlagsFromSampleCount(m_Description.ColourTargetSampleCount);

		VkPipelineMultisampleStateCreateInfo info = {};
		info.sType								  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.pNext								  = nullptr;
		info.sampleShadingEnable				  = VK_FALSE;
		info.minSampleShading					  = 0.0f;
		info.rasterizationSamples				  = samples;
		info.minSampleShading					  = 1.0f;
		info.pSampleMask						  = nullptr;
		info.alphaToCoverageEnable				  = VK_FALSE;
		info.alphaToOneEnable					  = VK_FALSE;
		return info;
	}

	std::vector<VkPipelineColorBlendAttachmentState> GraphicsPipelineVk::CreateColorBlendAttachmentStates()
	{
		std::vector<VkPipelineColorBlendAttachmentState> colourBlendStates;

		for (size_t i = 0; i < m_Description.ColourTargetCount; i++)
		{
			VkColorComponentFlags writeMask = {};
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Red)
			{
				writeMask |= VK_COLOR_COMPONENT_R_BIT;
			}
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Green)
			{
				writeMask |= VK_COLOR_COMPONENT_G_BIT;
			}
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Blue)
			{
				writeMask |= VK_COLOR_COMPONENT_B_BIT;
			}
			if (m_Description.ColourBlendStates[i].PixelWriteMask.Alpha)
			{
				writeMask |= VK_COLOR_COMPONENT_A_BIT;
			}

			VkPipelineColorBlendAttachmentState blendState = {};
			blendState.colorWriteMask					   = writeMask;
			blendState.blendEnable						   = m_Description.ColourBlendStates[i].EnableBlending;
			blendState.srcColorBlendFactor				   = Vk::GetVkBlendFactor(m_Description.ColourBlendStates[i].SourceColourBlend);
			blendState.dstColorBlendFactor				   = Vk::GetVkBlendFactor(m_Description.ColourBlendStates[i].DestinationColourBlend);
			blendState.colorBlendOp						   = Vk::GetVkBlendOp(m_Description.ColourBlendStates[i].ColorBlendFunction);
			blendState.srcAlphaBlendFactor				   = Vk::GetVkBlendFactor(m_Description.ColourBlendStates[i].SourceAlphaBlend);
			blendState.dstAlphaBlendFactor				   = Vk::GetVkBlendFactor(m_Description.ColourBlendStates[i].DestinationAlphaBlend);
			blendState.alphaBlendOp						   = Vk::GetVkBlendOp(m_Description.ColourBlendStates[i].AlphaBlendFunction);
			colourBlendStates.push_back(blendState);
		}

		return colourBlendStates;
	}

	VkPipelineDepthStencilStateCreateInfo GraphicsPipelineVk::CreatePipelineDepthStencilStateCreateInfo()
	{
		VkPipelineDepthStencilStateCreateInfo info = {};
		info.sType								   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.pNext								   = nullptr;
		info.depthTestEnable					   = m_Description.DepthStencilDesc.EnableDepthTest;
		info.depthWriteEnable					   = m_Description.DepthStencilDesc.EnableDepthWrite;
		info.depthCompareOp						   = Vk::GetCompareOp(m_Description.DepthStencilDesc.DepthComparisonFunction);

		info.depthBoundsTestEnable = VK_FALSE;
		// info.minDepthBounds		   = m_Description.DepthStencilDesc.MinDepth;
		// info.maxDepthBounds		   = m_Description.DepthStencilDesc.MaxDepth;

		info.stencilTestEnable = m_Description.DepthStencilDesc.EnableStencilTest;

		info.back.writeMask	  = m_Description.DepthStencilDesc.StencilWriteMask;
		info.back.compareMask = m_Description.DepthStencilDesc.StencilCompareMask;
		info.back.reference	  = m_Description.DepthStencilDesc.StencilReference;
		info.back.compareOp	  = Vk::GetCompareOp(m_Description.DepthStencilDesc.Back.StencilComparisonFunction);
		info.back.failOp	  = Vk::GetStencilOp(m_Description.DepthStencilDesc.Back.StencilFailOperation);
		info.back.passOp	  = Vk::GetStencilOp(m_Description.DepthStencilDesc.Back.StencilSuccessDepthSuccessOperation);
		info.back.depthFailOp = Vk::GetStencilOp(m_Description.DepthStencilDesc.Back.StencilSuccessDepthFailOperation);

		info.front.writeMask   = m_Description.DepthStencilDesc.StencilWriteMask;
		info.front.compareMask = m_Description.DepthStencilDesc.StencilCompareMask;
		info.front.reference   = m_Description.DepthStencilDesc.StencilReference;
		info.front.compareOp   = Vk::GetCompareOp(m_Description.DepthStencilDesc.Front.StencilComparisonFunction);
		info.front.failOp	   = Vk::GetStencilOp(m_Description.DepthStencilDesc.Front.StencilFailOperation);
		info.front.passOp	   = Vk::GetStencilOp(m_Description.DepthStencilDesc.Front.StencilSuccessDepthSuccessOperation);
		info.front.depthFailOp = Vk::GetStencilOp(m_Description.DepthStencilDesc.Front.StencilSuccessDepthFailOperation);

		return info;
	}

	VkPrimitiveTopology GraphicsPipelineVk::GetPrimitiveTopology()
	{
		switch (m_Description.PrimitiveTopology)
		{
			case Nexus::Graphics::Topology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
			case Nexus::Graphics::Topology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
			case Nexus::Graphics::Topology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
			case Nexus::Graphics::Topology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
			case Nexus::Graphics::Topology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
			default: throw std::runtime_error("An invalid primitive topology was entered"); break;
		}

		return VkPrimitiveTopology();
	}

	VkPolygonMode GraphicsPipelineVk::GetPolygonMode()
	{
		switch (m_Description.RasterizerStateDesc.TriangleFillMode)
		{
			case Nexus::Graphics::FillMode::Solid: return VK_POLYGON_MODE_FILL; break;
			case Nexus::Graphics::FillMode::Wireframe: return VK_POLYGON_MODE_LINE; break;
			default: throw std::runtime_error("An invalid fill mode was entered"); break;
		}
	}

	VkCullModeFlags GraphicsPipelineVk::GetCullMode()
	{
		switch (m_Description.RasterizerStateDesc.TriangleCullMode)
		{
			case Nexus::Graphics::CullMode::CullNone: return VK_CULL_MODE_NONE; break;
			case Nexus::Graphics::CullMode::Back: return VK_CULL_MODE_BACK_BIT; break;
			case Nexus::Graphics::CullMode::Front: return VK_CULL_MODE_FRONT_BIT; break;
			default: throw std::runtime_error("An invalid culling mode was entered"); break;
		}
	}

	VkPipelineShaderStageCreateInfo CreateShaderStageCreateInfo(Nexus::Ref<Nexus::Graphics::ShaderModuleVk> module)
	{
		VkPipelineShaderStageCreateInfo createInfo = {};
		createInfo.sType						   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.pNext						   = nullptr;
		createInfo.stage						   = Vk::GetVkShaderStageFlags(module->GetModuleSpecification().ShadingStage);
		createInfo.module						   = module->GetShaderModule();
		createInfo.pName						   = "main";
		return createInfo;
	}

	std::vector<VkPipelineShaderStageCreateInfo> GraphicsPipelineVk::GetShaderStages()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		if (m_Description.FragmentModule)
		{
			auto vulkanFragmentModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.FragmentModule);
			NX_ASSERT(vulkanFragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
			shaderStages.push_back(CreateShaderStageCreateInfo(vulkanFragmentModule));
		}

		if (m_Description.GeometryModule)
		{
			auto vulkanGeometryModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.GeometryModule);
			NX_ASSERT(vulkanGeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader");
			shaderStages.push_back(CreateShaderStageCreateInfo(vulkanGeometryModule));
		}

		if (m_Description.TesselationControlModule)
		{
			auto vulkanTesselationControlModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.TesselationControlModule);
			NX_ASSERT(vulkanTesselationControlModule->GetShaderStage() == ShaderStage::TesselationControl,
					  "Shader module is not a tesselation control shader");
			shaderStages.push_back(CreateShaderStageCreateInfo(vulkanTesselationControlModule));
		}

		if (m_Description.TesselationEvaluationModule)
		{
			auto vulkanTesselationEvaluation = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.TesselationEvaluationModule);
			NX_ASSERT(vulkanTesselationEvaluation->GetShaderStage() == ShaderStage::TesselationEvaluation,
					  "Shader module is not a tesselation evaluation shader");
			shaderStages.push_back(CreateShaderStageCreateInfo(vulkanTesselationEvaluation));
		}

		if (m_Description.VertexModule)
		{
			auto vulkanVertexModule = std::dynamic_pointer_cast<ShaderModuleVk>(m_Description.VertexModule);
			NX_ASSERT(vulkanVertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader");
			shaderStages.push_back(CreateShaderStageCreateInfo(vulkanVertexModule));
		}

		return shaderStages;
	}

	ComputePipelineVk::ComputePipelineVk(const ComputePipelineDescription &description, GraphicsDeviceVk *graphicsDevice)
		: ComputePipeline(description),
		  m_GraphicsDevice(graphicsDevice)
	{
		NX_ASSERT(description.ComputeShader->GetShaderStage() == ShaderStage::Compute,
				  "Shader passed to ComputePipelineDescription was not a compute shader");

		std::unique_ptr<ResourceSetVk> resourceSet = std::make_unique<ResourceSetVk>(description.ResourceSetSpec, graphicsDevice);

		const auto						  &pipelineLayouts = resourceSet->GetDescriptorSetLayouts();
		std::vector<VkDescriptorSetLayout> layouts;
		for (const auto &layout : pipelineLayouts) { layouts.push_back(layout.second); }

		VkPipelineLayoutCreateInfo layoutInfo = CreatePipelineLayoutCreateInfo(layouts);
		if (vkCreatePipelineLayout(graphicsDevice->GetVkDevice(), &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
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
	}

	ComputePipelineVk::~ComputePipelineVk()
	{
	}

	void ComputePipelineVk::Bind(VkCommandBuffer cmd)
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