#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"

	#include "DeviceBufferVk.hpp"
	#include "CommandListVk.hpp"
	#include "FramebufferVk.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "PipelineVk.hpp"
	#include "ResourceSetVk.hpp"
	#include "SamplerVk.hpp"
	#include "ShaderModuleVk.hpp"
	#include "TextureVk.hpp"
	#include "TimingQueryVk.hpp"
	#include "SwapchainVk.hpp"
	#include "PlatformVk.hpp"
	#include "PhysicalDeviceVk.hpp"
	#include "DeviceBufferVk.hpp"

namespace Nexus::Graphics
{
	GraphicsDeviceVk::GraphicsDeviceVk(std::shared_ptr<IPhysicalDevice> physicalDevice, VkInstance instance)
		: m_PhysicalDevice(std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice)),
		  m_Instance(instance)
	{
		std::shared_ptr<PhysicalDeviceVk> physicalDeviceVk = std::dynamic_pointer_cast<PhysicalDeviceVk>(physicalDevice);
		CreateDevice(physicalDeviceVk);
		auto deviceExtensions = GetSupportedDeviceExtensions(physicalDeviceVk);
		CreateAllocator(physicalDeviceVk, instance);

		CreateCommandStructures();
		CreateSynchronisationStructures();

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

	void GraphicsDeviceVk::SubmitCommandList(Ref<CommandList> commandList)
	{
		VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		Ref<CommandListVk>	 commandListVk	   = std::dynamic_pointer_cast<CommandListVk>(commandList);

		vkWaitForFences(m_Device, 1, &commandListVk->GetCurrentFence(), VK_TRUE, 0);
		vkResetFences(m_Device, 1, &commandListVk->GetCurrentFence());

		const std::vector<Nexus::Graphics::RenderCommandData> &commands = commandListVk->GetCommandData();
		m_CommandExecutor->SetCommandBuffer(commandListVk->GetCurrentCommandBuffer());
		m_CommandExecutor->ExecuteCommands(commands, this);
		m_CommandExecutor->Reset();

		VkSubmitInfo submitInfo			= {};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount	= 0;
		submitInfo.pWaitSemaphores		= nullptr;
		submitInfo.pWaitDstStageMask	= &waitDestStageMask;
		submitInfo.commandBufferCount	= 1;
		submitInfo.pCommandBuffers		= &commandListVk->GetCurrentCommandBuffer();
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores	= nullptr;

		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, commandListVk->GetCurrentFence()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit queue");
		}

		vkWaitForFences(m_Device, 1, &commandListVk->GetCurrentFence(), VK_TRUE, UINT32_MAX);
		vkResetFences(m_Device, 1, &commandListVk->GetCurrentFence());
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

	Ref<CommandList> GraphicsDeviceVk::CreateCommandList(const CommandListSpecification &spec)
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

	Ref<Sampler> GraphicsDeviceVk::CreateSampler(const SamplerSpecification &spec)
	{
		return CreateRef<SamplerVk>(this, spec);
	}

	Ref<TimingQuery> GraphicsDeviceVk::CreateTimingQuery()
	{
		return CreateRef<TimingQueryVk>(this);
	}

	DeviceBuffer *GraphicsDeviceVk::CreateDeviceBuffer(const DeviceBufferDescription &desc)
	{
		return new DeviceBufferVk(desc, this);
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

	Swapchain *GraphicsDeviceVk::CreateSwapchain(IWindow *window, const SwapchainSpecification &spec)
	{
		SwapchainVk						 *swapchain		   = new SwapchainVk(window, this, spec);
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

	Texture *GraphicsDeviceVk::CreateTexture(const TextureSpecification &spec)
	{
		return new TextureVk(spec, this);
	}

	ShaderLanguage GraphicsDeviceVk::GetSupportedShaderFormat()
	{
		return ShaderLanguage::SPIRV;
	}

	void GraphicsDeviceVk::WaitForIdle()
	{
		vkDeviceWaitIdle(m_Device);
	}

	GraphicsAPI GraphicsDeviceVk::GetGraphicsAPI()
	{
		return GraphicsAPI::Vulkan;
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

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy		= VK_TRUE;
		deviceFeatures.sampleRateShading		= VK_TRUE;
		deviceFeatures.independentBlend			= VK_TRUE;

		VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
		dynamicRenderingFeatures.sType									  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
		dynamicRenderingFeatures.pNext									  = nullptr;
		dynamicRenderingFeatures.dynamicRendering						  = VK_TRUE;

		VkDeviceCreateInfo createInfo	   = {};
		createInfo.sType				   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext				   = &dynamicRenderingFeatures;
		createInfo.pQueueCreateInfos	   = &queueCreateInfo;
		createInfo.queueCreateInfoCount	   = queueCreateInfos.size();
		createInfo.pQueueCreateInfos	   = queueCreateInfos.data();
		createInfo.pEnabledFeatures		   = &deviceFeatures;
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
		extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
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

	bool GraphicsDeviceVk::Validate()
	{
		return m_Device != VK_NULL_HANDLE && m_Allocator != VK_NULL_HANDLE;
	}

	void GraphicsDeviceVk::SetName(const std::string &name)
	{
		GraphicsDevice::SetName(name);

		/* std::string instanceName = name + std::string(" - Instance");
		Vk::SetObjectName(m_Device, VK_OBJECT_TYPE_INSTANCE, (uint64_t)m_Instance, instanceName.c_str());

		std::string physicalDeviceName = name + std::string(" - PhysicalDevice");
		Vk::SetObjectName(m_Device, VK_OBJECT_TYPE_PHYSICAL_DEVICE, (uint64_t)m_PhysicalDevice, physicalDeviceName.c_str()); */

		std::string deviceName = name + std::string(" - Device");
		Vk::SetObjectName(m_Device, VK_OBJECT_TYPE_DEVICE, (uint64_t)m_Device, deviceName.c_str());

		/* if (m_Specification.DebugLayer)
		{
			std::string debugName = name + std::string(" - Debug Messenger");
			Vk::SetObjectName(m_Device, VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT, (uint64_t)m_DebugMessenger, debugName.c_str());
		} */

		std::string uploadFenceName = name + std::string(" - Upload Context Fence");
		Vk::SetObjectName(m_Device, VK_OBJECT_TYPE_FENCE, (uint64_t)m_UploadContext.UploadFence, uploadFenceName.c_str());

		std::string uploadCmdPoolName = name + std::string(" - Upload Command Pool");
		Vk::SetObjectName(m_Device, VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)m_UploadContext.CommandPool, uploadCmdPoolName.c_str());

		std::string uploadCmdBufferName = name + std::string(" - Upload Commamd Buffer");
		Vk::SetObjectName(m_Device, VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)m_UploadContext.UploadFence, uploadCmdBufferName.c_str());
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