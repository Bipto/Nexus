#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"

	#include "CommandListVk.hpp"
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
	#include "TextureVk.hpp"
	#include "TimingQueryVk.hpp"
	#include "AccelerationStructureVk.hpp"

namespace Nexus::Graphics
{
	GraphicsDeviceVk::GraphicsDeviceVk(std::shared_ptr<IPhysicalDevice> physicalDevice, VkInstance instance, const VulkanDeviceConfig &config)
		: m_PhysicalDevice(std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice)),
		  m_Instance(instance),
		  m_DeviceConfig(config)
	{
		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice);
		CreateDevice(physicalDeviceVk);
		auto deviceExtensions = GetSupportedDeviceExtensions(physicalDeviceVk);
		CreateAllocator(physicalDeviceVk, instance);

		CreateCommandStructures();
		CreateSynchronisationStructures();

		LoadExtensionFunctions();

		m_CommandExecutor = std::make_unique<CommandExecutorVk>(this);
	}

	GraphicsDeviceVk::~GraphicsDeviceVk()
	{
		// cleanup synchronisation structures
		{
			vkDestroyFence(m_Device, m_UploadContext.UploadFence, nullptr);
			vkFreeCommandBuffers(m_Device, m_UploadContext.CommandPool, 1, &m_UploadContext.CommandBuffer);
			vkDestroyCommandPool(m_Device, m_UploadContext.CommandPool, nullptr);
		}

		// cleanup allocators
		{
			vmaDestroyAllocator(m_Allocator);
		}

		// cleanup device
		{
			vkDestroyDevice(m_Device, nullptr);
		}
	}

	void GraphicsDeviceVk::SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence)
	{
		VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		Ref<CommandListVk>									   commandListVk = std::dynamic_pointer_cast<CommandListVk>(commandList);
		const std::vector<Nexus::Graphics::RenderCommandData> &commands		 = commandList->GetCommandData();
		m_CommandExecutor->SetCommandBuffer(commandListVk->GetCurrentCommandBuffer());
		m_CommandExecutor->ExecuteCommands(commands, this);
		m_CommandExecutor->Reset();
		VkCommandBuffer vkCmdBuffer = commandListVk->GetCurrentCommandBuffer();

		if (m_ExtensionFunctions.vkQueueSubmit2KHR)
		{
			VkCommandBufferSubmitInfoKHR commandBufferInfo = {};
			commandBufferInfo.sType						   = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
			commandBufferInfo.pNext						   = nullptr;
			commandBufferInfo.commandBuffer				   = vkCmdBuffer;
			commandBufferInfo.deviceMask				   = 0;

			VkSubmitInfo2KHR submitInfo			= {};
			submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
			submitInfo.pNext					= nullptr;
			submitInfo.flags					= 0;
			submitInfo.waitSemaphoreInfoCount	= 0;
			submitInfo.pWaitSemaphoreInfos		= nullptr;
			submitInfo.commandBufferInfoCount	= 1;
			submitInfo.pCommandBufferInfos		= &commandBufferInfo;
			submitInfo.signalSemaphoreInfoCount = 0;
			submitInfo.pSignalSemaphoreInfos	= nullptr;

			VkFence vulkanFence = nullptr;

			if (fence)
			{
				Ref<FenceVk> apiFence = std::dynamic_pointer_cast<FenceVk>(fence);
				vulkanFence			  = apiFence->GetHandle();
			}

			NX_ASSERT(m_ExtensionFunctions.vkQueueSubmit2KHR(m_GraphicsQueue, 1, &submitInfo, vulkanFence) == VK_SUCCESS, "Failed to submit queue");
		}
		else
		{
			VkSubmitInfo submitInfo			= {};
			submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount	= 0;
			submitInfo.pWaitSemaphores		= nullptr;
			submitInfo.pWaitDstStageMask	= &waitDestStageMask;
			submitInfo.commandBufferCount	= 1;
			submitInfo.pCommandBuffers		= &vkCmdBuffer;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores	= nullptr;

			VkFence vulkanFence = nullptr;

			if (fence)
			{
				Ref<FenceVk> apiFence = std::dynamic_pointer_cast<FenceVk>(fence);
				vulkanFence			  = apiFence->GetHandle();
			}

			NX_ASSERT(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, vulkanFence) == VK_SUCCESS, "Failed to submit queue");
		}
	}

	void GraphicsDeviceVk::SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence)
	{
		VkPipelineStageFlags		 waitDestStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		std::vector<VkCommandBuffer> commandBuffers(numCommandLists);

		// record the commands into the actual vulkan command list
		for (uint32_t i = 0; i < numCommandLists; i++)
		{
			Ref<CommandListVk>									   commandList = std::dynamic_pointer_cast<CommandListVk>(commandLists[i]);
			const std::vector<Nexus::Graphics::RenderCommandData> &commands	   = commandList->GetCommandData();
			m_CommandExecutor->SetCommandBuffer(commandList->GetCurrentCommandBuffer());
			m_CommandExecutor->ExecuteCommands(commands, this);
			m_CommandExecutor->Reset();
			commandBuffers[i] = commandList->GetCurrentCommandBuffer();
		}

		if (m_ExtensionFunctions.vkQueueSubmit2KHR)
		{
			std::vector<VkCommandBufferSubmitInfoKHR> commandBufferInfos = {};

			for (VkCommandBuffer commandBuffer : commandBuffers)
			{
				VkCommandBufferSubmitInfoKHR &commandBufferInfo = commandBufferInfos.emplace_back();
				commandBufferInfo.sType							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR;
				commandBufferInfo.pNext							= nullptr;
				commandBufferInfo.commandBuffer					= commandBuffer;
				commandBufferInfo.deviceMask					= 0;
			}

			VkSubmitInfo2KHR submitInfo			= {};
			submitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR;
			submitInfo.pNext					= nullptr;
			submitInfo.flags					= 0;
			submitInfo.waitSemaphoreInfoCount	= 0;
			submitInfo.pWaitSemaphoreInfos		= nullptr;
			submitInfo.commandBufferInfoCount	= commandBufferInfos.size();
			submitInfo.pCommandBufferInfos		= commandBufferInfos.data();
			submitInfo.signalSemaphoreInfoCount = 0;
			submitInfo.pSignalSemaphoreInfos	= nullptr;

			VkFence vulkanFence = nullptr;

			if (fence)
			{
				Ref<FenceVk> apiFence = std::dynamic_pointer_cast<FenceVk>(fence);
				vulkanFence			  = apiFence->GetHandle();
			}

			NX_ASSERT(m_ExtensionFunctions.vkQueueSubmit2KHR(m_GraphicsQueue, 1, &submitInfo, vulkanFence) == VK_SUCCESS, "Failed to submit queue");
		}
		else
		{
			VkSubmitInfo submitInfo			= {};
			submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount	= 0;
			submitInfo.pWaitSemaphores		= nullptr;
			submitInfo.pWaitDstStageMask	= &waitDestStageMask;
			submitInfo.commandBufferCount	= commandBuffers.size();
			submitInfo.pCommandBuffers		= commandBuffers.data();
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores	= nullptr;

			VkFence vkFence = nullptr;

			if (fence)
			{
				Ref<FenceVk> apiFence = std::dynamic_pointer_cast<FenceVk>(fence);
				vkFence				  = apiFence->GetHandle();
			}

			NX_ASSERT(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, vkFence) == VK_SUCCESS, "Failed to submit queue");
		}
	}

	const std::string GraphicsDeviceVk::GetAPIName()
	{
		return "Vulkan";
	}

	const char *GraphicsDeviceVk::GetDeviceName()
	{
		return nullptr;
	}

	std::shared_ptr<IPhysicalDevice> GraphicsDeviceVk::GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	Ref<ShaderModule> GraphicsDeviceVk::CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources)
	{
		return CreateRef<ShaderModuleVk>(moduleSpec, resources, this);
	}

	Ref<GraphicsPipeline> GraphicsDeviceVk::CreateGraphicsPipeline(const GraphicsPipelineDescription &description)
	{
		return CreateRef<GraphicsPipelineVk>(description, this);
	}

	Ref<ComputePipeline> GraphicsDeviceVk::CreateComputePipeline(const ComputePipelineDescription &description)
	{
		return CreateRef<ComputePipelineVk>(description, this);
	}

	Ref<MeshletPipeline> GraphicsDeviceVk::CreateMeshletPipeline(const MeshletPipelineDescription &description)
	{
		return CreateRef<MeshletPipelineVk>(description, this);
	}

	Ref<RayTracingPipeline> GraphicsDeviceVk::CreateRayTracingPipeline(const RayTracingPipelineDescription &description)
	{
		return nullptr;
	}

	Ref<CommandList> GraphicsDeviceVk::CreateCommandList(const CommandListDescription &spec)
	{
		return CreateRef<CommandListVk>(this, spec);
	}

	Ref<ResourceSet> GraphicsDeviceVk::CreateResourceSet(const ResourceSetSpecification &spec)
	{
		return CreateRef<ResourceSetVk>(spec, this);
	}

	Ref<Framebuffer> GraphicsDeviceVk::CreateFramebuffer(const FramebufferSpecification &spec)
	{
		return CreateRef<FramebufferVk>(spec, this);
	}

	Ref<Sampler> GraphicsDeviceVk::CreateSampler(const SamplerDescription &spec)
	{
		return CreateRef<SamplerVk>(this, spec);
	}

	Ref<TimingQuery> GraphicsDeviceVk::CreateTimingQuery()
	{
		return CreateRef<TimingQueryVk>(this);
	}

	Ref<DeviceBuffer> GraphicsDeviceVk::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return CreateRef<DeviceBufferVk>(desc, this);
	}

	Ref<IAccelerationStructure> GraphicsDeviceVk::CreateAccelerationStructure(const AccelerationStructureDescription &desc)
	{
		return CreateRef<AccelerationStructureVk>(desc, this);
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

	Ref<Swapchain> GraphicsDeviceVk::CreateSwapchain(IWindow *window, const SwapchainSpecification &spec)
	{
		Ref<SwapchainVk>				  swapchain		   = CreateRef<SwapchainVk>(window, this, spec);
		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(m_PhysicalDevice);

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDeviceVk->GetVkPhysicalDevice(),
											 m_PresentQueueFamilyIndex,
											 swapchain->m_Surface,
											 &presentSupport);

		if (!presentSupport)
		{
			throw std::runtime_error("Device is unable to present to this swapchain");
		}

		return swapchain;
	}

	Ref<Fence> GraphicsDeviceVk::CreateFence(const FenceDescription &desc)
	{
		return CreateRef<FenceVk>(desc, this);
	}

	FenceWaitResult GraphicsDeviceVk::WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout)
	{
		std::vector<VkFence> fenceHandles(count);
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceVk> fence = std::dynamic_pointer_cast<FenceVk>(fences[i]);
			fenceHandles[i]	   = fence->GetHandle();
		}

		VkResult result = vkWaitForFences(m_Device, fenceHandles.size(), fenceHandles.data(), waitAll, timeout.GetNanoseconds<uint64_t>());

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

	void GraphicsDeviceVk::ResetFences(Ref<Fence> *fences, uint32_t count)
	{
		std::vector<VkFence> fenceHandles(count);
		for (uint32_t i = 0; i < count; i++)
		{
			Ref<FenceVk> fence = std::dynamic_pointer_cast<FenceVk>(fences[i]);
			fenceHandles[i]	   = fence->GetHandle();
		}

		VkResult result = vkResetFences(m_Device, fenceHandles.size(), fenceHandles.data());
		NX_ASSERT(result == VK_SUCCESS, "Failed to reset fences");
	}

	Ref<Texture> GraphicsDeviceVk::CreateTexture(const TextureDescription &spec)
	{
		return CreateRef<TextureVk>(spec, this);
	}

	ShaderLanguage GraphicsDeviceVk::GetSupportedShaderFormat()
	{
		return ShaderLanguage::SPIRV;
	}

	bool GraphicsDeviceVk::IsBufferUsageSupported(BufferUsage usage)
	{
		return false;
	}

	void GraphicsDeviceVk::WaitForIdle()
	{
		vkDeviceWaitIdle(m_Device);
	}

	GraphicsAPI GraphicsDeviceVk::GetGraphicsAPI()
	{
		return GraphicsAPI::Vulkan;
	}

	void GraphicsDeviceVk::SetObjectName(VkObjectType type, uint64_t handle, const char *name)
	{
		VkDebugUtilsObjectNameInfoEXT nameInfo = {};
		nameInfo.sType						   = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		nameInfo.pNext						   = nullptr;
		nameInfo.objectType					   = type;
		nameInfo.objectHandle				   = handle;
		nameInfo.pObjectName				   = name;

		if (m_ExtensionFunctions.vkSetDebugUtilsObjectNameEXT)
		{
			m_ExtensionFunctions.vkSetDebugUtilsObjectNameEXT(m_Device, &nameInfo);
		}
	}

	const DeviceExtensionFunctions &GraphicsDeviceVk::GetExtensionFunctions() const
	{
		return m_ExtensionFunctions;
	}

	VkInstance GraphicsDeviceVk::GetVkInstance()
	{
		return m_Instance;
	}

	VkDevice GraphicsDeviceVk::GetVkDevice()
	{
		return m_Device;
	}

	uint32_t GraphicsDeviceVk::GetGraphicsFamily()
	{
		return m_GraphicsQueueFamilyIndex;
	}

	uint32_t GraphicsDeviceVk::GetPresentFamily()
	{
		return m_PresentQueueFamilyIndex;
	}

	uint32_t GraphicsDeviceVk::GetCurrentFrameIndex()
	{
		return m_FrameNumber % FRAMES_IN_FLIGHT;
	}

	VmaAllocator GraphicsDeviceVk::GetAllocator()
	{
		return m_Allocator;
	}

	void GraphicsDeviceVk::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function)
	{
		vkResetFences(m_Device, 1, &m_UploadContext.UploadFence);
		vkResetCommandPool(m_Device, m_UploadContext.CommandPool, 0);

		VkCommandBuffer			 cmd		  = m_UploadContext.CommandBuffer;
		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType					  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext					  = nullptr;
		cmdBeginInfo.pInheritanceInfo		  = nullptr;
		cmdBeginInfo.flags					  = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(cmd, &cmdBeginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin command buffer");
		}

		function(cmd);

		if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end command buffer");
		}

		VkSubmitInfo submitInfo			= {};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext				= nullptr;
		submitInfo.waitSemaphoreCount	= 0;
		submitInfo.pWaitSemaphores		= nullptr;
		submitInfo.pWaitDstStageMask	= nullptr;
		submitInfo.commandBufferCount	= 1;
		submitInfo.pCommandBuffers		= &cmd;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores	= nullptr;

		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_UploadContext.UploadFence) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit queue");
		}

		vkDeviceWaitIdle(m_Device);
		vkQueueWaitIdle(m_GraphicsQueue);
	}

	void GraphicsDeviceVk::TransitionVulkanImageLayout(VkCommandBuffer		 cmdBuffer,
													   VkImage				 image,
													   uint32_t				 mipLevel,
													   uint32_t				 arrayLayer,
													   VkImageLayout		 oldLayout,
													   VkImageLayout		 newLayout,
													   VkImageAspectFlagBits aspectMask)
	{
		if (oldLayout == newLayout)
		{
			return;
		}

		VkImageMemoryBarrier barrier {};
		barrier.sType	  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image							= image;
		barrier.subresourceRange.aspectMask		= aspectMask;
		barrier.subresourceRange.baseMipLevel	= mipLevel;
		barrier.subresourceRange.levelCount		= 1;
		barrier.subresourceRange.baseArrayLayer = arrayLayer;
		barrier.subresourceRange.layerCount		= 1;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		vkCmdPipelineBarrier(cmdBuffer,
							 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
							 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
							 VK_DEPENDENCY_BY_REGION_BIT,
							 0,
							 nullptr,
							 0,
							 nullptr,
							 1,
							 &barrier);
	}

	void GraphicsDeviceVk::SelectQueueFamilies(std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;
		uint32_t							 queueFamilyCount;

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->GetVkPhysicalDevice(), &queueFamilyCount, nullptr);
		queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice->GetVkPhysicalDevice(), &queueFamilyCount, queueFamilyProperties.data());

		int graphicsIndex = -1;
		int presentIndex  = -1;

		int i = 0;
		for (const auto &queueFamily : queueFamilyProperties)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsIndex = i;
			}

			if (queueFamily.queueCount > 0)
			{
				presentIndex = i;
			}

			if (graphicsIndex != -1 && presentIndex != -1)
			{
				break;
			}
			i++;
		}

		if (graphicsIndex == -1 || presentIndex == -1)
		{
			throw std::runtime_error("Failed to find a graphics or present queue");
		}

		m_GraphicsQueueFamilyIndex = graphicsIndex;
		m_PresentQueueFamilyIndex  = presentIndex;
	}

	void GraphicsDeviceVk::CreateDevice(std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		SelectQueueFamilies(physicalDevice);

		std::vector<const char *> deviceExtensions = GetRequiredDeviceExtensions();
		const float				  queuePriority[]  = {1.0f};

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t>					 uniqueQueueFamilies = {m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex};

		float priority = queuePriority[0];
		for (int queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex		= queueFamily;
			queueCreateInfo.queueCount				= 1;
			queueCreateInfo.pQueuePriorities		= &priority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex		= m_GraphicsQueueFamilyIndex;
		queueCreateInfo.queueCount				= 1;
		queueCreateInfo.pQueuePriorities		= &priority;

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

		VkPhysicalDeviceSynchronization2FeaturesKHR synchronizationFeatures = {};
		synchronizationFeatures.sType										= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
		synchronizationFeatures.pNext										= nullptr;
		synchronizationFeatures.synchronization2							= VK_TRUE;
		if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME))
		{
			builder.Add(synchronizationFeatures);
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

		createInfo.pQueueCreateInfos	   = &queueCreateInfo;
		createInfo.queueCreateInfoCount	   = queueCreateInfos.size();
		createInfo.pQueueCreateInfos	   = queueCreateInfos.data();
		createInfo.enabledExtensionCount   = deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.enabledLayerCount	   = 0;

		if (vkCreateDevice(physicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create device");
		}

		vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_PresentQueueFamilyIndex, 0, &m_PresentQueue);
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

	void GraphicsDeviceVk::CreateCommandStructures()
	{
		// upload command pool
		{
			VkCommandPoolCreateInfo createInfo = {};
			createInfo.sType				   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.flags				   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			createInfo.queueFamilyIndex		   = m_GraphicsQueueFamilyIndex;
			if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_UploadContext.CommandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create command pool");
			}
		}

		// upload command buffer
		{
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType						 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.commandPool				 = m_UploadContext.CommandPool;
			allocateInfo.level						 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount			 = 1;

			if (vkAllocateCommandBuffers(m_Device, &allocateInfo, &m_UploadContext.CommandBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate command buffers");
			}
		}
	}

	void GraphicsDeviceVk::CreateSynchronisationStructures()
	{
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType			  = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags			  = VK_FENCE_CREATE_SIGNALED_BIT;

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType				  = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.flags				  = 0;

		if (vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_UploadContext.UploadFence) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create fence");
		}
	}

	void GraphicsDeviceVk::LoadExtensionFunctions()
	{
		m_ExtensionFunctions.vkCmdBindVertexBuffers2EXT = (PFN_vkCmdBindVertexBuffers2EXT)vkGetDeviceProcAddr(m_Device, "vkCmdBindVertexBuffers2EXT");

		m_ExtensionFunctions.vkCmdBindIndexBuffer2KHR  = (PFN_vkCmdBindIndexBuffer2KHR)vkGetDeviceProcAddr(m_Device, "vkCmdBindIndexBuffer2KHR");
		m_ExtensionFunctions.vkCmdDebugMarkerBeginEXT  = (PFN_vkCmdDebugMarkerBeginEXT)vkGetDeviceProcAddr(m_Device, "vkCmdDebugMarkerBeginEXT");
		m_ExtensionFunctions.vkCmdDebugMarkerEndEXT	   = (PFN_vkCmdDebugMarkerEndEXT)vkGetDeviceProcAddr(m_Device, "vkCmdDebugMarkerEndEXT");
		m_ExtensionFunctions.vkCmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT)vkGetDeviceProcAddr(m_Device, "vkCmdDebugMarkerInsertEXT");

		m_ExtensionFunctions.vkCmdBeginDebugUtilsLabelEXT =
			(PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_Device, "vkCmdBeginDebugUtilsLabelEXT");
		m_ExtensionFunctions.vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_Device, "vkCmdEndDebugUtilsLabelEXT");
		m_ExtensionFunctions.vkCmdInsertDebugUtilsLabelEXT =
			(PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetDeviceProcAddr(m_Device, "vkCmdInsertDebugUtilsLabelEXT");
		m_ExtensionFunctions.vkSetDebugUtilsObjectNameEXT =
			(PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(m_Device, "vkSetDebugUtilsObjectNameEXT");

		m_ExtensionFunctions.vkCmdBeginRenderPass2KHR = (PFN_vkCmdBeginRenderPass2KHR)vkGetDeviceProcAddr(m_Device, "vkCmdBeginRenderPass2KHR");
		m_ExtensionFunctions.vkCmdEndRenderPass2KHR	  = (PFN_vkCmdEndRenderPass2KHR)vkGetDeviceProcAddr(m_Device, "vkCmdEndRenderPass2KHR");
		m_ExtensionFunctions.vkCreateRenderPass2KHR	  = (PFN_vkCreateRenderPass2KHR)vkGetDeviceProcAddr(m_Device, "vkCreateRenderPass2KHR");
		m_ExtensionFunctions.vkDebugMarkerSetObjectNameEXT =
			(PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(m_Device, "vkDebugMarkerSetObjectNameEXT");

		m_ExtensionFunctions.vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR)vkGetDeviceProcAddr(m_Device, "vkCmdBeginRenderingKHR");
		m_ExtensionFunctions.vkCmdEndRenderingKHR	= (PFN_vkCmdEndRenderingKHR)vkGetDeviceProcAddr(m_Device, "vkCmdEndRenderingKHR");

		m_ExtensionFunctions.vkCmdDrawMeshTasksEXT = (PFN_vkCmdDrawMeshTasksEXT)vkGetDeviceProcAddr(m_Device, "vkCmdDrawMeshTasksEXT");
		m_ExtensionFunctions.vkCmdDrawMeshTasksIndirectEXT =
			(PFN_vkCmdDrawMeshTasksIndirectEXT)vkGetDeviceProcAddr(m_Device, "vkCmdDrawMeshTasksIndirectEXT");

		m_ExtensionFunctions.vkGetBufferDeviceAddressKHR =
			(PFN_vkGetBufferDeviceAddressKHR)vkGetDeviceProcAddr(m_Device, "vkGetBufferDeviceAddressKHR");

		m_ExtensionFunctions.vkGetAccelerationStructureBuildSizesKHR =
			(PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(m_Device, "vkGetAccelerationStructureBuildSizesKHR");

		m_ExtensionFunctions.vkCreateAccelerationStructureKHR =
			(PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(m_Device, "vkCreateAccelerationStructureKHR");
		m_ExtensionFunctions.vkDestroyAccelerationStructureKHR =
			(PFN_vkDestroyAccelerationStructureKHR)vkGetDeviceProcAddr(m_Device, "vkDestroyAccelerationStructureKHR");
		m_ExtensionFunctions.vkCmdBuildAccelerationStructuresKHR =
			(PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(m_Device, "vkCmdBuildAccelerationStructuresKHR");
		m_ExtensionFunctions.vkCmdCopyAccelerationStructureKHR =
			(PFN_vkCmdCopyAccelerationStructureKHR)vkGetDeviceProcAddr(m_Device, "vkCmdCopyAccelerationStructureKHR");
		m_ExtensionFunctions.vkCmdCopyAccelerationStructureToMemoryKHR =
			(PFN_vkCmdCopyAccelerationStructureToMemoryKHR)vkGetDeviceProcAddr(m_Device, "vkCmdCopyAccelerationStructureToMemoryKHR");
		m_ExtensionFunctions.vkCmdCopyMemoryToAccelerationStructureKHR =
			(PFN_vkCmdCopyMemoryToAccelerationStructureKHR)vkGetDeviceProcAddr(m_Device, "vkCmdCopyMemoryToAccelerationStructureKHR");
		m_ExtensionFunctions.vkGetAccelerationStructureDeviceAddressKHR =
			(PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(m_Device, "vkGetAccelerationStructureDeviceAddressKHR");

		m_ExtensionFunctions.vkQueueSubmit2KHR = (PFN_vkQueueSubmit2KHR)vkGetDeviceProcAddr(m_Device, "vkQueueSubmit2KHR");
	}

	VkImageView GraphicsDeviceVk::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo			 = {};
		viewInfo.sType							 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image							 = image;
		viewInfo.viewType						 = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format							 = format;
		viewInfo.subresourceRange.aspectMask	 = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel	 = 0;
		viewInfo.subresourceRange.levelCount	 = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount	 = 1;

		VkImageView imageView;
		if (vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view");
		}
		return imageView;
	}

	void GraphicsDeviceVk::CreateImage(uint32_t				 width,
									   uint32_t				 height,
									   VkFormat				 format,
									   VkImageTiling		 tiling,
									   VkImageUsageFlags	 usage,
									   VkMemoryPropertyFlags properties,
									   VkImage				&image,
									   VkDeviceMemory		&imageMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType			= VK_IMAGE_TYPE_2D;
		imageInfo.extent.width		= width;
		imageInfo.extent.height		= height;
		imageInfo.extent.depth		= 1;
		imageInfo.mipLevels			= 1;
		imageInfo.arrayLayers		= 1;
		imageInfo.format			= format;
		imageInfo.tiling			= tiling;
		imageInfo.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage				= usage;
		imageInfo.samples			= VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(m_PhysicalDevice);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType				   = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize	   = memRequirements.size;
		allocInfo.memoryTypeIndex	   = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDeviceVk);

		if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory");
		}

		vkBindImageMemory(m_Device, image, imageMemory, 0);
	}

	std::vector<const char *> GraphicsDeviceVk::GetRequiredDeviceExtensions()
	{
		std::vector<const char *> extensions;
		extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensions.push_back(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME);

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

		return extensions;
	}

	std::vector<std::string> GraphicsDeviceVk::GetSupportedDeviceExtensions(std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		uint32_t count	= 0;
		VkResult result = vkEnumerateDeviceExtensionProperties(physicalDevice->GetVkPhysicalDevice(), nullptr, &count, nullptr);

		std::vector<VkExtensionProperties> properties(count);
		result = vkEnumerateDeviceExtensionProperties(physicalDevice->GetVkPhysicalDevice(), nullptr, &count, properties.data());

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
		return m_ExtensionFunctions.vkGetBufferDeviceAddressKHR(m_Device, &info);
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
		const AccelerationStructureGeometryBuildDescription &description,
		const std::vector<uint32_t>							&primitiveCount) const
	{
		std::vector<VkAccelerationStructureGeometryKHR> geometries = Vk::GetVulkanAccelerationStructureGeometries(description);
		VkAccelerationStructureBuildGeometryInfoKHR		buildInfo  = Vk::GetGeometryBuildInfo(description, geometries);

		VkAccelerationStructureBuildSizesInfoKHR buildSizes = {};
		buildSizes.sType									= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		buildSizes.pNext									= nullptr;

		if (m_ExtensionFunctions.vkGetAccelerationStructureBuildSizesKHR)
		{
			m_ExtensionFunctions.vkGetAccelerationStructureBuildSizesKHR(m_Device,
																		 VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR,
																		 &buildInfo,
																		 primitiveCount.data(),
																		 &buildSizes);
		}

		return AccelerationStructureBuildSizeDescription {.AccelerationStructureSize = buildSizes.accelerationStructureSize,
														  .UpdateScratchSize		 = buildSizes.updateScratchSize,
														  .BuildScratchSize			 = buildSizes.buildScratchSize};
	}

	bool GraphicsDeviceVk::IsExtensionSupported(const char *extension) const
	{
		return m_PhysicalDevice->IsExtensionSupported(extension);
	}

	bool GraphicsDeviceVk::IsVersionGreaterThan(uint32_t version) const
	{
		return m_PhysicalDevice->IsVersionGreaterThan(version);
	}

	uint32_t GraphicsDeviceVk::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, std::shared_ptr<PhysicalDeviceVk> physicalDevice)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice->GetVkPhysicalDevice(), &memProperties);

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

#endif