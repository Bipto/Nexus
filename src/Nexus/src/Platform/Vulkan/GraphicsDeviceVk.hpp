#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "CommandExecutorVk.hpp"
	#include "PhysicalDeviceVk.hpp"
	#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
	#include "SwapchainVk.hpp"
	#include "DeviceBufferVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	struct UploadContext
	{
		VkFence			UploadFence;
		VkCommandPool	CommandPool;
		VkCommandBuffer CommandBuffer;
	};

	struct VulkanDeviceConfig
	{
		bool Debug							= false;
		bool UseDynamicRenderingIfAvailable = false;
	};

	struct VulkanDeviceFeatures
	{
		bool DynamicRenderingAvailable = false;
		bool Supports8BitIndices	   = false;
		bool SupportsRayTracing		   = false;
	};

	struct VulkanDeviceExtensionFunctions
	{
		PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT = VK_NULL_HANDLE;
		PFN_vkCmdBindIndexBuffer2KHR   vkCmdBindIndexBuffer2KHR	  = VK_NULL_HANDLE;

		// these are the more modern debug functions, so use these if available
		PFN_vkCmdBeginDebugUtilsLabelEXT  vkCmdBeginDebugUtilsLabelEXT	= VK_NULL_HANDLE;
		PFN_vkCmdEndDebugUtilsLabelEXT	  vkCmdEndDebugUtilsLabelEXT	= VK_NULL_HANDLE;
		PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = VK_NULL_HANDLE;
		PFN_vkSetDebugUtilsObjectNameEXT  vkSetDebugUtilsObjectNameEXT	= VK_NULL_HANDLE;

		// otherwise, fall back to these
		PFN_vkCmdDebugMarkerBeginEXT	  vkCmdDebugMarkerBeginEXT		= VK_NULL_HANDLE;
		PFN_vkCmdDebugMarkerEndEXT		  vkCmdDebugMarkerEndEXT		= VK_NULL_HANDLE;
		PFN_vkCmdDebugMarkerInsertEXT	  vkCmdDebugMarkerInsertEXT		= VK_NULL_HANDLE;
		PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT = VK_NULL_HANDLE;

		PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR = VK_NULL_HANDLE;
		PFN_vkCmdEndRenderPass2KHR	 vkCmdEndRenderPass2KHR	  = VK_NULL_HANDLE;
		PFN_vkCreateRenderPass2KHR	 vkCreateRenderPass2KHR	  = VK_NULL_HANDLE;

		PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR = VK_NULL_HANDLE;
		PFN_vkCmdEndRenderingKHR   vkCmdEndRenderingKHR	  = VK_NULL_HANDLE;

		PFN_vkCmdDrawMeshTasksEXT		  vkCmdDrawMeshTasksEXT			= VK_NULL_HANDLE;
		PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT = VK_NULL_HANDLE;

		PFN_vkGetBufferDeviceAddressKHR				   vkGetBufferDeviceAddressKHR				  = VK_NULL_HANDLE;
		PFN_vkGetAccelerationStructureBuildSizesKHR	   vkGetAccelerationStructureBuildSizesKHR	  = VK_NULL_HANDLE;
		PFN_vkCreateAccelerationStructureKHR		   vkCreateAccelerationStructureKHR			  = VK_NULL_HANDLE;
		PFN_vkDestroyAccelerationStructureKHR		   vkDestroyAccelerationStructureKHR		  = VK_NULL_HANDLE;
		PFN_vkCmdBuildAccelerationStructuresKHR		   vkCmdBuildAccelerationStructuresKHR		  = VK_NULL_HANDLE;
		PFN_vkCmdCopyAccelerationStructureKHR		   vkCmdCopyAccelerationStructureKHR		  = VK_NULL_HANDLE;
		PFN_vkCmdCopyAccelerationStructureToMemoryKHR  vkCmdCopyAccelerationStructureToMemoryKHR  = VK_NULL_HANDLE;
		PFN_vkCmdCopyMemoryToAccelerationStructureKHR  vkCmdCopyMemoryToAccelerationStructureKHR  = VK_NULL_HANDLE;
		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = VK_NULL_HANDLE;

		PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR = VK_NULL_HANDLE;
		PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR = VK_NULL_HANDLE;
		PFN_vkGetDeviceQueue2	   vkGetDeviceQueue2	  = VK_NULL_HANDLE;
	};

	class GraphicsDeviceVk final : public GraphicsDevice
	{
	  public:
		GraphicsDeviceVk(std::shared_ptr<IPhysicalDevice> physicalDevice, VkInstance instance, const VulkanDeviceConfig &config);
		GraphicsDeviceVk(const GraphicsDeviceVk &) = delete;
		virtual ~GraphicsDeviceVk();

		const std::string				 GetAPIName() final;
		std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const final;

		Ref<GraphicsPipeline>		CreateGraphicsPipeline(const GraphicsPipelineDescription &description) final;
		Ref<ComputePipeline>		CreateComputePipeline(const ComputePipelineDescription &description) final;
		Ref<MeshletPipeline>		CreateMeshletPipeline(const MeshletPipelineDescription &description) final;
		Ref<RayTracingPipeline>		CreateRayTracingPipeline(const RayTracingPipelineDescription &description) final;

		Ref<CommandList>			CreateCommandList(const CommandListDescription &spec = {}) final;
		Ref<ResourceSet>			CreateResourceSet(Ref<Pipeline> pipeline) final;
		Ref<Framebuffer>			CreateFramebuffer(const FramebufferSpecification &spec) final;
		Ref<Sampler>				CreateSampler(const SamplerDescription &spec) final;
		Ref<TimingQuery>			CreateTimingQuery() final;
		Ref<DeviceBuffer>			CreateDeviceBuffer(const DeviceBufferDescription &desc) final;
		Ref<IAccelerationStructure> CreateAccelerationStructure(const AccelerationStructureDescription &desc) final;

		const GraphicsCapabilities GetGraphicsCapabilities() const final;
		Ref<Texture>			   CreateTexture(const TextureDescription &spec) final;
		Ref<Swapchain>			   CreateSwapchain(IWindow *window, const SwapchainDescription &spec) final;
		Ref<Fence>				   CreateFence(const FenceDescription &desc) final;
		FenceWaitResult			   WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) final;
		std::vector<QueueFamilyInfo> GetQueueFamilies() final;
		Ref<ICommandQueue>			 CreateCommandQueue(const CommandQueueDescription &description) final;
		void						 ResetFences(Ref<Fence> *fences, uint32_t count) final;

		ShaderLanguage GetSupportedShaderFormat() final;
		bool		   IsBufferUsageSupported(BufferUsage usage) final;
		void		   WaitForIdle() final;
		float		   GetUVCorrection() final
		{
			return -1.0f;
		}
		bool IsUVOriginTopLeft() final
		{
			return true;
		};

		GraphicsAPI GetGraphicsAPI() final;

		void							SetObjectName(VkObjectType type, uint64_t handle, const char *name);
		const VulkanDeviceExtensionFunctions &GetExtensionFunctions() const;

		VkInstance	 GetVkInstance();
		VkDevice	 GetVkDevice();
		uint32_t	 GetGraphicsFamily();
		uint32_t	 GetPresentFamily();
		uint32_t	 GetCurrentFrameIndex();
		VmaAllocator GetAllocator();

		void				ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function);
		void				TransitionVulkanImageLayout(VkCommandBuffer		  cmdBuffer,
														VkImage				  image,
														uint32_t			  mipLevel,
														uint32_t			  arrayLayer,
														VkImageLayout		  oldLayout,
														VkImageLayout		  newLayout,
														VkImageAspectFlagBits aspectMask);
		uint32_t			FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		Vk::AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

		const VulkanDeviceFeatures GetDeviceFeatures() const;

		bool Validate() final;

		PixelFormatProperties					  GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const final;
		const DeviceFeatures					 &GetPhysicalDeviceFeatures() const final;
		const DeviceLimits						 &GetPhysicalDeviceLimits() const final;
		bool									  IsIndexBufferFormatSupported(IndexFormat format) const final;
		AccelerationStructureBuildSizeDescription GetAccelerationStructureBuildSize(const AccelerationStructureGeometryBuildDescription &description,
																					const std::vector<uint32_t> &primitiveCount) const final;

		bool IsExtensionSupported(const char *extension) const;
		bool IsVersionGreaterThan(uint32_t version) const;

		// vulkan functions
	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec) override;

		void RetrieveQueueFamilies(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		void SelectQueueFamilies(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		void CreateDevice(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		void CreateAllocator(std::shared_ptr<PhysicalDeviceVk> physicalDevice, VkInstance instance);

		void CreateCommandStructures();
		void CreateSynchronisationStructures();

		void LoadExtensionFunctions();

	  private:
		// utility functions
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void		CreateImage(uint32_t			  width,
								uint32_t			  height,
								VkFormat			  format,
								VkImageTiling		  tiling,
								VkImageUsageFlags	  usage,
								VkMemoryPropertyFlags properties,
								VkImage				 &image,
								VkDeviceMemory		 &imageMemory);

		std::vector<const char *> GetRequiredDeviceExtensions();
		std::vector<std::string>  GetSupportedDeviceExtensions(std::shared_ptr<PhysicalDeviceVk> physicalDevice);

		VkDeviceAddress GetBufferDeviceAddress(Ref<DeviceBufferVk> buffer);

	  private:
		std::shared_ptr<PhysicalDeviceVk> m_PhysicalDevice = nullptr;
		VkInstance						  m_Instance	   = nullptr;

		std::vector<QueueFamilyInfo> m_QueueFamilies = {};

		uint32_t m_GraphicsQueueFamilyIndex;
		uint32_t m_PresentQueueFamilyIndex;

		VkDevice m_Device;
		VkQueue	 m_GraphicsQueue;
		VkQueue	 m_PresentQueue;

		// VMA types
		VmaAllocator m_Allocator;

		// vulkan upload context
		UploadContext m_UploadContext;

		uint32_t						   m_FrameNumber	   = 0;
		uint32_t						   m_CurrentFrameIndex = 0;
		std::unique_ptr<CommandExecutorVk> m_CommandExecutor   = nullptr;

		VulkanDeviceConfig	 m_DeviceConfig	  = {};
		VulkanDeviceFeatures m_DeviceFeatures = {};

		DeviceFeatures m_Features = {};
		DeviceLimits   m_Limits	  = {};

		VulkanDeviceExtensionFunctions m_ExtensionFunctions = {};

		friend class SwapchainVk;
	};
}	 // namespace Nexus::Graphics

#endif