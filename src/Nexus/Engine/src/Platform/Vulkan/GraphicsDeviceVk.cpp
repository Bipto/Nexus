#include "GraphicsDeviceVk.hpp"

#include "AccelerationStructureVk.hpp"
#include "CommandListVk.hpp"
#include "CommandQueueVk.hpp"
#include "DeviceBufferVk.hpp"
#include "FenceVk.hpp"
#include "FramebufferVk.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "PhysicalDeviceVk.hpp"
#include "PipelineVk.hpp"
#include "PlatformVk.hpp"
#include "ResourceSetVk.hpp"
#include "SamplerVk.hpp"
#include "ShaderModuleVk.hpp"
#include "SwapchainVk.hpp"
#include "TexelBufferVk.hpp"
#include "TextureViewVk.hpp"
#include "TextureVk.hpp"
#include "TimingQueryVk.hpp"

#if defined(WIN32)
	#include "Surface/SurfaceWin32_Vk.hpp"
#elif defined(__linux__)
	#include "Surface/SurfaceX11_Vk.hpp"
#endif

#include "Platform/Timings/Profiler.hpp"

namespace Nexus::Graphics
{
	GraphicsDeviceVk::GraphicsDeviceVk(std::shared_ptr<IPhysicalDevice> physicalDevice, VkInstance instance, const VulkanDeviceConfig &config)
		: m_PhysicalDevice(std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice)),
		  m_Instance(instance),
		  m_DeviceConfig(config)
	{
		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice);

		Vk::GladLoaderData loaderData = {.instance = m_Instance, .device = m_Device};
		gladLoadVulkanContextUserPtr(&m_Context,
									 physicalDeviceVk->GetVkPhysicalDevice(),
									 (GLADuserptrloadfunc)Vk::GladFunctionLoaderWithInstance,
									 &loaderData);

		CreateDevice(physicalDeviceVk);

		auto deviceExtensions = GetSupportedDeviceExtensions(physicalDeviceVk);
		CreateAllocator(physicalDeviceVk, instance);
		m_CommandExecutor = std::make_unique<CommandExecutorVk>(this);

		uint32_t version = 0;
		m_Context.EnumerateInstanceVersion(&version);

		uint32_t major = VK_VERSION_MAJOR(version);
		uint32_t minor = VK_VERSION_MINOR(version);

		m_GraphicsAPIInfo = {
			.API   = GraphicsAPI::Vulkan,
			.Major = major,
			.Minor = minor,
		};
	}

	GraphicsDeviceVk::~GraphicsDeviceVk()
	{
		// cleanup allocators
		{
			vmaDestroyAllocator(m_Allocator);
		}

		// cleanup device
		{
			m_Context.DestroyDevice(m_Device, nullptr);
		}
	}

	std::shared_ptr<IPhysicalDevice> GraphicsDeviceVk::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	ShaderModuleHandle GraphicsDeviceVk::CreateShaderModule(const ShaderModuleDescription &moduleSpec)
	{
		auto shader = std::make_unique<ShaderModuleVk>(moduleSpec, this);
		return m_Resources.ShaderModules.CreateShared(std::move(shader));
	}

	Ref<IGraphicsPipeline> GraphicsDeviceVk::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		return CreateRef<GraphicsPipelineVk>(description, this);
	}

	Ref<IComputePipeline> GraphicsDeviceVk::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		return CreateRef<ComputePipelineVk>(description, this);
	}

	Ref<IMeshletPipeline> GraphicsDeviceVk::CreateMeshletPipeline(const MeshletPipelineDescription &description)
	{
		return CreateRef<MeshletPipelineVk>(description, this);
	}

	Ref<IRayTracingPipeline> GraphicsDeviceVk::CreateRayTracingPipeline(const RayTracingPipelineDescription &description)
	{
		return CreateRef<RayTracingPipelineVk>(description, this);
	}

	ResourceSetHandle GraphicsDeviceVk::CreateResourceSet(Ref<IPipeline> pipeline)
	{
		auto resourceSet = std::make_unique<ResourceSetVk>(pipeline, this);
		return m_Resources.ResourceSets.CreateShared(std::move(resourceSet));
	}

	FramebufferHandle GraphicsDeviceVk::CreateFramebuffer(const FramebufferTextureSetDescription &desc)
	{
		auto framebuffer = std::make_unique<FramebufferVk>(desc, this);
		return m_Resources.Framebuffers.CreateShared(std::move(framebuffer));
	}

	SamplerHandle GraphicsDeviceVk::CreateSampler(const SamplerDescription &spec)
	{
		// return CreateRef<SamplerVk>(this, spec);
		auto sampler = std::make_unique<SamplerVk>(this, spec);
		return m_Resources.Samplers.CreateShared(std::move(sampler));
	}

	TimingQueryHandle GraphicsDeviceVk::CreateTimingQuery()
	{
		auto timingQuery = std::make_unique<TimingQueryVk>(this);
		return m_Resources.TimingQueries.CreateShared(std::move(timingQuery));
	}

	Ref<IDeviceBuffer> GraphicsDeviceVk::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return CreateRef<DeviceBufferVk>(desc, this);
	}

	AccelerationStructureHandle GraphicsDeviceVk::CreateAccelerationStructure(const AccelerationStructureDescription &desc)
	{
		auto accelerationStructure = std::make_unique<AccelerationStructureVk>(desc, this);
		return m_Resources.AccelerationStructures.CreateShared(std::move(accelerationStructure));
	}

	TexelBufferHandle GraphicsDeviceVk::CreateTexelBuffer(const TexelBufferDescription &desc)
	{
		auto texelBuffer = std::make_unique<TexelBufferVk>(desc, this);
		return m_Resources.TexelBuffers.CreateShared(std::move(texelBuffer));
	}

	const GraphicsCapabilities GraphicsDeviceVk::GetGraphicsCapabilities() const
	{
		GraphicsCapabilities capabilities;
		capabilities.SupportsMultisampledTextures		 = true;
		capabilities.SupportsLODBias					 = true;
		capabilities.SupportsInstanceOffset				 = true;
		capabilities.SupportsMultipleSwapchains			 = true;
		capabilities.SupportsSeparateColourAndBlendMasks = true;
		return capabilities;
	}

	FenceHandle GraphicsDeviceVk::CreateFence(const FenceDescription &desc)
	{
		auto fence = std::make_unique<FenceVk>(desc, this);
		return m_Resources.Fences.CreateShared(std::move(fence));
	}

	FenceWaitResult GraphicsDeviceVk::WaitForFences(FenceHandle *fences, uint32_t count, bool waitAll, uint64_t timeoutNS)
	{
		std::vector<VkFence> fenceHandles(count);
		for (uint32_t i = 0; i < count; i++)
		{
			const FenceVk *fence = fences[i].AsDerived<const FenceVk>();
			fenceHandles[i]		 = fence->GetHandle();
		}

		VkResult result = m_Context.WaitForFences(m_Device, fenceHandles.size(), fenceHandles.data(), waitAll, timeoutNS);

		if (result == VK_SUCCESS)
		{
			return FenceWaitResult::Signalled;
		}
		else if (result == VK_TIMEOUT)
		{
			return FenceWaitResult::TimedOut;
		}
		else
		{
			return FenceWaitResult::Failed;
		}
	}

	std::vector<QueueFamilyInfo> GraphicsDeviceVk::GetQueueFamilies()
	{
		return m_QueueFamilies;
	}

	Ref<ICommandQueue> GraphicsDeviceVk::CreateCommandQueue(const CommandQueueDescription &description)
	{
		return CreateRef<CommandQueueVk>(this, description);
	}

	void GraphicsDeviceVk::ResetFences(FenceHandle *fences, uint32_t count)
	{
		std::vector<VkFence> fenceHandles(count);
		for (uint32_t i = 0; i < count; i++)
		{
			const FenceVk *fence = fences[i].AsDerived<const FenceVk>();
			fenceHandles[i]		 = fence->GetHandle();
		}

		VkResult result = m_Context.ResetFences(m_Device, fenceHandles.size(), fenceHandles.data());
		NX_VALIDATE(result == VK_SUCCESS, "Failed to reset fences");
	}

	TextureHandle GraphicsDeviceVk::CreateTexture(const TextureDescription &spec)
	{
		auto texture = std::make_unique<TextureVk>(spec, this);
		return m_Resources.Textures.CreateShared(std::move(texture));
	}

	TextureViewHandle GraphicsDeviceVk::CreateTextureView(const TextureViewDescription &desc)
	{
		auto textureView = std::make_unique<TextureViewVk>(desc, this);
		return m_Resources.TextureViews.CreateShared(std::move(textureView));
	}

	ShaderLanguage GraphicsDeviceVk::GetSupportedShaderFormat()
	{
		return ShaderLanguage::Vulkan_SPIRV;
	}

	bool GraphicsDeviceVk::IsBufferUsageSupported(BufferUsage usage)
	{
		return false;
	}

	void GraphicsDeviceVk::WaitForIdle()
	{
		m_Context.DeviceWaitIdle(m_Device);
	}

	GraphicsAPIInfo GraphicsDeviceVk::GetGraphicsAPI()
	{
		return m_GraphicsAPIInfo;
	}

	void GraphicsDeviceVk::SetObjectName(VkObjectType type, uint64_t handle, const char *name)
	{
		VkDebugUtilsObjectNameInfoEXT nameInfo = {};
		nameInfo.sType						   = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		nameInfo.pNext						   = nullptr;
		nameInfo.objectType					   = type;
		nameInfo.objectHandle				   = handle;
		nameInfo.pObjectName				   = name;

		if (m_Context.SetDebugUtilsObjectNameEXT)
		{
			m_Context.SetDebugUtilsObjectNameEXT(m_Device, &nameInfo);
		}
	}

	VkInstance GraphicsDeviceVk::GetVkInstance()
	{
		return m_Instance;
	}

	VkDevice GraphicsDeviceVk::GetVkDevice()
	{
		return m_Device;
	}

	uint32_t GraphicsDeviceVk::GetCurrentFrameIndex()
	{
		return m_FrameNumber % FRAMES_IN_FLIGHT;
	}

	VmaAllocator GraphicsDeviceVk::GetAllocator()
	{
		return m_Allocator;
	}

	void GraphicsDeviceVk::RetrieveQueueFamilies(std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		m_QueueFamilies.clear();

		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		uint32_t							 queueFamilyCount;

		m_Context.GetPhysicalDeviceQueueFamilyProperties(physicalDevice->GetVkPhysicalDevice(), &queueFamilyCount, nullptr);
		queueFamilyProperties.resize(queueFamilyCount);
		m_Context.GetPhysicalDeviceQueueFamilyProperties(physicalDevice->GetVkPhysicalDevice(), &queueFamilyCount, queueFamilyProperties.data());

		uint32_t queueFamilyIndex = 0;
		for (const auto &queueFamily : queueFamilyProperties)
		{
			QueueFamilyInfo &info = m_QueueFamilies.emplace_back();
			info.QueueFamily	  = queueFamilyIndex++;
			info.QueueCount		  = queueFamily.queueCount;
			info.Capabilities	  = Vk::GetNxQueueCapabilitiesFromVkQueuePropertyFlags(queueFamily.queueFlags);
		}
	}

	void GraphicsDeviceVk::CreateDevice(std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		RetrieveQueueFamilies(physicalDevice);

		std::vector<const char *> deviceExtensions = GetRequiredDeviceExtensions();

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
		std::vector<std::vector<float>>		 queuePriorities  = {};

		for (size_t i = 0; i < m_QueueFamilies.size(); i++)
		{
			const QueueFamilyInfo &queueFamilyInfo = m_QueueFamilies.at(i);

			uint32_t queueFamilyIndex = (uint32_t)i;
			uint32_t queueCount		  = queueFamilyInfo.QueueCount;

			queuePriorities.emplace_back(queueCount, 1.0f);

			VkDeviceQueueCreateInfo &queueCreateInfo = queueCreateInfos.emplace_back();
			queueCreateInfo.sType					 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex		 = i;
			queueCreateInfo.queueCount				 = queueFamilyInfo.QueueCount;
			queueCreateInfo.pQueuePriorities		 = queuePriorities.back().data();
		}

		Vk::PNextBuilder builder = {};

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy		= VK_TRUE;
		deviceFeatures.sampleRateShading		= VK_TRUE;
		deviceFeatures.independentBlend			= VK_TRUE;
		deviceFeatures.depthBounds				= VK_TRUE;

		VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
		deviceFeatures2.sType					  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures2.pNext					  = nullptr;
		deviceFeatures2.features				  = deviceFeatures;

		// we need to check if VkPhysicalDeviceFeatures2 is supported
		if (m_PhysicalDevice->IsVersionGreaterThan(VK_VERSION_1_1) ||
			m_PhysicalDevice->IsExtensionSupported(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		{
			builder.Add(deviceFeatures2);
		}

		VkPhysicalDeviceIndexTypeUint8FeaturesEXT indexType8Features = {};
		indexType8Features.sType									 = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT;
		indexType8Features.pNext									 = nullptr;
		indexType8Features.indexTypeUint8							 = VK_TRUE;
		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME))
		{
			builder.Add(indexType8Features);
		}

		VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures = {};
		extendedDynamicStateFeatures.sType				  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
		extendedDynamicStateFeatures.pNext				  = nullptr;
		extendedDynamicStateFeatures.extendedDynamicState = VK_TRUE;
		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME))
		{
			builder.Add(extendedDynamicStateFeatures);
		}

		VkPhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures = {};
		meshShaderFeatures.sType								 = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
		meshShaderFeatures.pNext								 = nullptr;
		meshShaderFeatures.taskShader							 = VK_TRUE;
		meshShaderFeatures.meshShader							 = VK_TRUE;
		if (m_Features.SupportsMeshTaskShaders)
		{
			builder.Add(meshShaderFeatures);
		}

		VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
		dynamicRenderingFeatures.sType									  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
		dynamicRenderingFeatures.pNext									  = nullptr;
		dynamicRenderingFeatures.dynamicRendering						  = VK_TRUE;
		if (m_DeviceConfig.UseDynamicRenderingIfAvailable && m_PhysicalDevice->IsExtensionSupported(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME))
		{
			builder.Add(dynamicRenderingFeatures);
		}

		VkPhysicalDeviceBufferDeviceAddressFeaturesEXT bufferDeviceAddressFeatures = {};
		bufferDeviceAddressFeatures.sType				= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
		bufferDeviceAddressFeatures.pNext				= nullptr;
		bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;
		if (IsVersionGreaterThan(VK_VERSION_1_2) || IsExtensionSupported(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
		{
			builder.Add(bufferDeviceAddressFeatures);
		}

		VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
		accelerationStructureFeatures.sType					= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		accelerationStructureFeatures.pNext					= nullptr;
		accelerationStructureFeatures.accelerationStructure = VK_TRUE;
		if (m_DeviceFeatures.SupportsRayTracing)
		{
			builder.Add(accelerationStructureFeatures);
		}

		VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures = {};
		rayTracingPipelineFeatures.sType			  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		rayTracingPipelineFeatures.pNext			  = nullptr;
		rayTracingPipelineFeatures.rayTracingPipeline = VK_TRUE;
		if (m_DeviceFeatures.SupportsRayTracing)
		{
			builder.Add(rayTracingPipelineFeatures);
		}

		VkPhysicalDeviceSynchronization2FeaturesKHR synchronizationFeatures = {};
		synchronizationFeatures.sType										= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
		synchronizationFeatures.pNext										= nullptr;
		synchronizationFeatures.synchronization2							= VK_TRUE;
		if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME))
		{
			builder.Add(synchronizationFeatures);
		}

		VkPhysicalDeviceInlineUniformBlockFeaturesEXT inlineUniformBlockFeatures = {};
		inlineUniformBlockFeatures.sType			  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT;
		inlineUniformBlockFeatures.pNext			  = nullptr;
		inlineUniformBlockFeatures.inlineUniformBlock = VK_TRUE;
		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME))
		{
			builder.Add(inlineUniformBlockFeatures);
		}

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType			  = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext			  = nullptr;

		// we need to check if VkPhysicalDeviceFeatures2 is supported
		if (m_PhysicalDevice->IsVersionGreaterThan(VK_VERSION_1_1) ||
			m_PhysicalDevice->IsExtensionSupported(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		{
			createInfo.pNext			= builder.GetHead();
			createInfo.pEnabledFeatures = nullptr;
		}
		else
		{
			createInfo.pNext			= nullptr;
			createInfo.pEnabledFeatures = &deviceFeatures;
		}

		createInfo.pQueueCreateInfos	   = queueCreateInfos.data();
		createInfo.queueCreateInfoCount	   = queueCreateInfos.size();
		createInfo.enabledExtensionCount   = deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.enabledLayerCount	   = 0;

		if (m_Context.CreateDevice(physicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create device");
		}

		// load device function pointers
		Vk::GladLoaderData loaderData = {.instance = m_Instance, .device = m_Device};
		gladLoadVulkanContextUserPtr(&m_Context,
									 m_PhysicalDevice->GetVkPhysicalDevice(),
									 (GLADuserptrloadfunc)Vk::GladFunctionLoaderWithInstance,
									 &loaderData);
	}

	void GraphicsDeviceVk::CreateAllocator(std::shared_ptr<PhysicalDeviceVk> physicalDevice, VkInstance instance)
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice		 = physicalDevice->GetVkPhysicalDevice();
		allocatorInfo.device				 = m_Device;
		allocatorInfo.instance				 = instance;

		if (IsVersionGreaterThan(VK_VERSION_1_2) || IsExtensionSupported(VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
		{
			allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		}

		if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create VMA allocator");
		}
	}

	std::vector<const char *> GraphicsDeviceVk::GetRequiredDeviceExtensions()
	{
		std::vector<const char *> extensions;
		extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_DEVICE_GROUP_EXTENSION_NAME))
		{
			extensions.push_back(VK_KHR_DEVICE_GROUP_EXTENSION_NAME);

			// incremental present extension to support custom present rectangles
			if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME))
			{
				extensions.push_back(VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME);
			}

			if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME))
			{
				extensions.push_back(VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME);

				// swapchain maintenance1 extension to support changing present modes without recreating the swapchain
				if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME))
				{
					extensions.push_back(VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME);
				}
			}
		}

		// this is used for vkCmdBindVertexBuffers2	{
		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME))
		{
			extensions.push_back(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
		}

		// this is used for dynamic rendering
		if (m_DeviceConfig.UseDynamicRenderingIfAvailable)
		{
			// dynamic rendering has a dependency on depth/stencil resolve
			if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME))
			{
				extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
				m_DeviceFeatures.DynamicRenderingAvailable = true;

				if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME))
				{
					extensions.push_back(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);

					// this is used for vkCmdBindIndexBuffer2 and requires dynamic rendering
					if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_MAINTENANCE_5_EXTENSION_NAME))
					{
						extensions.push_back(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
					}
				}
			}
		}

		// this is used for vkCreateRenderPass2
		if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
		{
			extensions.push_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
		}

		// mesh shaders
		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_MESH_SHADER_EXTENSION_NAME))
		{
			extensions.push_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);

			if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SPIRV_1_4_EXTENSION_NAME))
			{
				extensions.push_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);

				if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME))
				{
					extensions.push_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

					if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_MAINTENANCE_4_EXTENSION_NAME))
					{
						extensions.push_back(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
						m_Features.SupportsMeshTaskShaders = true;
					}
				}
			}
		}

		// 8 bit indices
		{
			if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME))
			{
				extensions.push_back(VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME);
				m_DeviceFeatures.Supports8BitIndices = true;
			}
		}

		// this is used to set debug object names and groups
		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		}

		if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME))
		{
			extensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
		}

		// ray tracing
		{
			if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
			{
				extensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
			}

			if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME))
			{
				extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);

				if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
				{
					extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);

					if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
					{
						extensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
						m_DeviceFeatures.SupportsRayTracing = true;
					}
				}
			}
		}

		if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME))
		{
			extensions.push_back(VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME);
		}

		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME))
		{
			extensions.push_back(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME);
		}

		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME))
		{
			extensions.push_back(VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME);
		}

		if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_MAINTENANCE_6_EXTENSION_NAME))
		{
			extensions.push_back(VK_KHR_MAINTENANCE_6_EXTENSION_NAME);
		}

		return extensions;
	}

	std::vector<std::string> GraphicsDeviceVk::GetSupportedDeviceExtensions(std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		uint32_t count	= 0;
		VkResult result = m_Context.EnumerateDeviceExtensionProperties(physicalDevice->GetVkPhysicalDevice(), nullptr, &count, nullptr);

		std::vector<VkExtensionProperties> properties(count);
		result = m_Context.EnumerateDeviceExtensionProperties(physicalDevice->GetVkPhysicalDevice(), nullptr, &count, properties.data());

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to get supported device extensions");
		}

		std::vector<std::string> extensions;
		for (const auto &property : properties) { extensions.push_back(property.extensionName); }

		return extensions;
	}

	VkDeviceAddress GraphicsDeviceVk::GetBufferDeviceAddress(Ref<DeviceBufferVk> buffer)
	{
		VkBufferDeviceAddressInfo info = {};
		info.sType					   = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		info.pNext					   = nullptr;
		info.buffer					   = buffer->GetVkBuffer();
		return m_Context.GetBufferDeviceAddressKHR(m_Device, &info);
	}

	Vk::AllocatedBuffer GraphicsDeviceVk::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType			  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext			  = nullptr;
		bufferInfo.size				  = allocSize;
		bufferInfo.usage			  = usage;

		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage					 = memoryUsage;
		vmaAllocInfo.flags					 = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		vmaAllocInfo.requiredFlags			 = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		Vk::AllocatedBuffer buffer;

		if (vmaCreateBuffer(m_Allocator, &bufferInfo, &vmaAllocInfo, &buffer.Buffer, &buffer.Allocation, nullptr) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create buffer");
		}

		return buffer;
	}

	const VulkanDeviceFeatures GraphicsDeviceVk::GetDeviceFeatures() const
	{
		return m_DeviceFeatures;
	}

	bool GraphicsDeviceVk::Validate()
	{
		return m_Device != VK_NULL_HANDLE && m_Allocator != VK_NULL_HANDLE;
	}

	PixelFormatProperties GraphicsDeviceVk::GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const
	{
		PixelFormatProperties properties = {};
		return properties;
	}

	const DeviceFeatures &GraphicsDeviceVk::GetPhysicalDeviceFeatures() const
	{
		return m_Features;
	}

	const DeviceLimits &GraphicsDeviceVk::GetPhysicalDeviceLimits() const
	{
		return m_Limits;
	}

	bool GraphicsDeviceVk::IsIndexBufferFormatSupported(IndexFormat format) const
	{
		switch (format)
		{
			case IndexFormat::UInt8: return m_DeviceFeatures.Supports8BitIndices;
			case IndexFormat::UInt16:
			case IndexFormat::UInt32: return true;
			default: throw std::runtime_error("Failed to find valid index buffer format");
		}
	}

	AccelerationStructureBuildSizeDescription GraphicsDeviceVk::GetAccelerationStructureBuildSize(
		const AccelerationStructureGeometryBuildDescription &description) const
	{
		std::vector<uint32_t>							primitiveCounts = {};
		std::vector<VkAccelerationStructureGeometryKHR> geometries		= Vk::GetVulkanAccelerationStructureGeometries(description, primitiveCounts);
		VkAccelerationStructureBuildGeometryInfoKHR		buildInfo		= Vk::GetGeometryBuildInfo(description, geometries);

		VkAccelerationStructureBuildSizesInfoKHR buildSizes = {};
		buildSizes.sType									= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		buildSizes.pNext									= nullptr;

		if (m_Context.GetAccelerationStructureBuildSizesKHR)
		{
			m_Context.GetAccelerationStructureBuildSizesKHR(m_Device,
															VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR,
															&buildInfo,
															primitiveCounts.data(),
															&buildSizes);
		}

		return AccelerationStructureBuildSizeDescription {.AccelerationStructureSize = buildSizes.accelerationStructureSize,
														  .UpdateScratchSize		 = buildSizes.updateScratchSize,
														  .BuildScratchSize			 = buildSizes.buildScratchSize};
	}

	RayTracingDeviceDescription GraphicsDeviceVk::GetRayTracingDeviceDescription() const
	{
		RayTracingDeviceDescription description = {};

		if (m_Context.GetPhysicalDeviceProperties2)
		{
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingProps = {};
			rayTracingProps.sType											= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

			VkPhysicalDeviceProperties2 props2 = {};
			props2.sType					   = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			props2.pNext					   = &rayTracingProps;

			m_Context.GetPhysicalDeviceProperties2(m_PhysicalDevice->GetVkPhysicalDevice(), &props2);

			description.ShaderGroupHandleSize			   = rayTracingProps.shaderGroupHandleSize;
			description.MaxRayRecursionDepth			   = rayTracingProps.maxRayRecursionDepth;
			description.MaxShaderGroupStride			   = rayTracingProps.maxShaderGroupStride;
			description.ShaderGroupBaseAlignment		   = rayTracingProps.shaderGroupBaseAlignment;
			description.ShaderGroupHandleCaptureReplaySize = rayTracingProps.shaderGroupHandleCaptureReplaySize;
			description.MaxRayDispatchInvocationCount	   = rayTracingProps.maxRayDispatchInvocationCount;
			description.ShaderGroupHandleAlignment		   = rayTracingProps.shaderGroupHandleAlignment;
			description.MaxRayHitAttributeSize			   = rayTracingProps.maxRayHitAttributeSize;
		}

		return description;
	}

	AccelerationStructureProperties GraphicsDeviceVk::GetAccelerationStructureProperties() const
	{
		AccelerationStructureProperties properties {};

		if (m_Context.GetPhysicalDeviceProperties2)
		{
			VkPhysicalDeviceAccelerationStructurePropertiesKHR accelStructProps = {};
			accelStructProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;

			VkPhysicalDeviceProperties2 props2 = {};
			props2.sType					   = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			props2.pNext					   = &accelStructProps;

			m_Context.GetPhysicalDeviceProperties2(m_PhysicalDevice->GetVkPhysicalDevice(), &props2);

			properties.MaxGeometryCount								  = accelStructProps.maxGeometryCount;
			properties.MaxInstanceCount								  = accelStructProps.maxInstanceCount;
			properties.MaxPrimitiveCount							  = accelStructProps.maxPrimitiveCount;
			properties.MinAccelerationStructureScratchOffsetAlignment = accelStructProps.minAccelerationStructureScratchOffsetAlignment;
		}

		return properties;
	}

	Ref<ISurface> GraphicsDeviceVk::CreateSurfaceFromWin32(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance) const
	{
#if defined(WIN32)
		return CreateRef<SurfaceWin32_Vk>(hwnd, hdc, hinstance);
#else
		return nullptr;
#endif
	}

	Ref<ISurface> GraphicsDeviceVk::CreateSurfaceFromX11(uintptr_t display, uint32_t screen, uint32_t window) const
	{
#if defined(__linux__)
		return CreateRef<SurfaceX11_Vk>(display, screen, window);
#else
		return nullptr;
#endif
	}

	Ref<ISurface> GraphicsDeviceVk::CreateSurfaceFromWayland(uintptr_t display, uintptr_t surface) const
	{
		return nullptr;
	}

	Ref<ISurface> GraphicsDeviceVk::CreateSurfaceFromAndroid(uintptr_t nativeWindow) const
	{
		return nullptr;
	}

	Ref<ISurface> GraphicsDeviceVk::CreateSurfaceFromHTML(const std::string &canvasId) const
	{
		return nullptr;
	}

	bool GraphicsDeviceVk::IsExtensionSupported(const char *extension) const
	{
		return m_PhysicalDevice->IsExtensionSupported(extension);
	}

	bool GraphicsDeviceVk::IsVersionGreaterThan(uint32_t version) const
	{
		return m_PhysicalDevice->IsVersionGreaterThan(version);
	}

	const GladVulkanContext &GraphicsDeviceVk::GetVulkanContext() const
	{
		return m_Context;
	}

	uint32_t GraphicsDeviceVk::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		m_Context.GetPhysicalDeviceMemoryProperties(physicalDevice->GetVkPhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties))
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}
}	 // namespace Nexus::Graphics