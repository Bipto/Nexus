#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "CommandExecutorVk.hpp"
	#include "PhysicalDeviceVk.hpp"
	#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
	#include "SwapchainVk.hpp"
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
	};

	struct DeviceExtensionFunctions
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
	};

	class GraphicsDeviceVk : public GraphicsDevice
	{
	  public:
		GraphicsDeviceVk(std::shared_ptr<IPhysicalDevice> physicalDevice, VkInstance instance, const VulkanDeviceConfig &config);
		GraphicsDeviceVk(const GraphicsDeviceVk &) = delete;
		virtual ~GraphicsDeviceVk();

		virtual void SubmitCommandList(Ref<CommandList> commandList, Ref<Fence> fence) override;
		virtual void SubmitCommandLists(Ref<CommandList> *commandLists, uint32_t numCommandLists, Ref<Fence> fence) override;

		virtual const std::string				 GetAPIName() override;
		virtual const char						*GetDeviceName() override;
		virtual std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const override;

		virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription &description) override;
		virtual Ref<ComputePipeline>  CreateComputePipeline(const ComputePipelineDescription &description) override;
		virtual Ref<CommandList>	  CreateCommandList(const CommandListDescription &spec = {}) override;
		virtual Ref<ResourceSet>	  CreateResourceSet(const ResourceSetSpecification &spec) override;
		virtual Ref<Framebuffer>	  CreateFramebuffer(const FramebufferSpecification &spec) override;
		virtual Ref<Sampler>		  CreateSampler(const SamplerDescription &spec) override;
		virtual Ref<TimingQuery>	  CreateTimingQuery() override;
		virtual Ref<DeviceBuffer>	  CreateDeviceBuffer(const DeviceBufferDescription &desc) override;

		virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;
		virtual Ref<Texture>			   CreateTexture(const TextureDescription &spec) override;
		virtual Ref<Swapchain>			   CreateSwapchain(IWindow *window, const SwapchainSpecification &spec) override;
		virtual Ref<Fence>				   CreateFence(const FenceDescription &desc) override;
		virtual FenceWaitResult			   WaitForFences(Ref<Fence> *fences, uint32_t count, bool waitAll, TimeSpan timeout) override;
		virtual void					   ResetFences(Ref<Fence> *fences, uint32_t count) override;

		virtual ShaderLanguage GetSupportedShaderFormat() override;
		virtual bool		   IsBufferUsageSupported(BufferUsage usage) override;
		virtual void		   WaitForIdle() override;
		virtual float		   GetUVCorrection() override
		{
			return -1.0f;
		}
		virtual bool IsUVOriginTopLeft() override
		{
			return true;
		};

		virtual GraphicsAPI GetGraphicsAPI() override;

		void							SetObjectName(VkObjectType type, uint64_t handle, const char *name);
		const DeviceExtensionFunctions &GetExtensionFunctions() const;

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

		virtual bool Validate() override;

		virtual PixelFormatProperties GetPixelFormatProperties(PixelFormat format, TextureType type, TextureUsageFlags usage) const override;
		virtual const DeviceFeatures &GetPhysicalDeviceFeatures() const override;
		virtual const DeviceLimits	 &GetPhysicalDeviceLimits() const override;
		virtual bool				  IsIndexBufferFormatSupported(IndexBufferFormat format) const override;

		// vulkan functions
	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;

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

	  private:
		std::shared_ptr<PhysicalDeviceVk> m_PhysicalDevice = nullptr;
		VkInstance						  m_Instance	   = nullptr;

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

		DeviceExtensionFunctions m_ExtensionFunctions = {};

		friend class SwapchainVk;
	};
}	 // namespace Nexus::Graphics

#endif