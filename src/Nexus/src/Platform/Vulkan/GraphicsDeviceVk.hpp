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

	class GraphicsDeviceVk : public GraphicsDevice
	{
	  public:
		GraphicsDeviceVk(std::shared_ptr<IPhysicalDevice> physicalDevice, VkInstance instance);
		GraphicsDeviceVk(const GraphicsDeviceVk &) = delete;
		virtual ~GraphicsDeviceVk();

		virtual void SubmitCommandList(Ref<CommandList> commandList) override;

		virtual const std::string				 GetAPIName() override;
		virtual const char						*GetDeviceName() override;
		virtual std::shared_ptr<IPhysicalDevice> GetPhysicalDevice() const override;

		virtual Ref<Texture2D>		  CreateTexture2D(const Texture2DSpecification &spec) override;
		virtual Ref<Cubemap>		  CreateCubemap(const CubemapSpecification &spec) override;
		virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription &description) override;
		virtual Ref<ComputePipeline>  CreateComputePipeline(const ComputePipelineDescription &description) override;
		virtual Ref<CommandList>	  CreateCommandList(const CommandListSpecification &spec = {}) override;
		virtual Ref<ResourceSet>	  CreateResourceSet(const ResourceSetSpecification &spec) override;
		virtual Ref<Framebuffer>	  CreateFramebuffer(const FramebufferSpecification &spec) override;
		virtual Ref<Sampler>		  CreateSampler(const SamplerSpecification &spec) override;
		virtual Ref<TimingQuery>	  CreateTimingQuery() override;
		virtual DeviceBuffer		 *CreateDeviceBuffer(const DeviceBufferDescription &desc) override;
		virtual void				  CopyBuffer(const BufferCopyDescription &desc) override;

		virtual const GraphicsCapabilities GetGraphicsCapabilities() const override;
		virtual Swapchain				  *CreateSwapchain(IWindow *window, const SwapchainSpecification &spec) override;

		virtual ShaderLanguage GetSupportedShaderFormat() override;
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

		virtual bool Validate() override;
		virtual void SetName(const std::string &name) override;

		// vulkan functions
	  private:
		virtual Ref<ShaderModule> CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources) override;

		void SelectQueueFamilies(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		void CreateDevice(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		void CreateAllocator(std::shared_ptr<PhysicalDeviceVk> physicalDevice, VkInstance instance);

		void CreateCommandStructures();
		void CreateSynchronisationStructures();

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

		friend class SwapchainVk;
	};
}	 // namespace Nexus::Graphics

#endif