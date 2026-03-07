#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/nxpch.hpp"
	#include "PhysicalDeviceVk.hpp"
	#include "RHI/Swapchain.hpp"
	#include "Surface/SurfaceVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	// forward declaration
	class GraphicsDeviceVk;
	class CommandQueueVk;
	class FramebufferVk;

	class SwapchainVk : public ISwapchain
	{
	  public:
		SwapchainVk(IGraphicsDevice *graphicsDevice, ICommandQueue *commandQueue, const SwapchainDescription &swapchainSpec);
		virtual ~SwapchainVk();

		void						  SwapBuffers(const SwapchainPresentDescription &presentDesc) final;
		Ref<IFramebuffer>			  GetCurrentFramebuffer() final;
		void						  SetPresentMode(PresentMode presentMode) final;
		std::pair<uint32_t, uint32_t> GetSize() final;

		PixelFormat GetColourFormat() final;
		PixelFormat GetDepthFormat() final;

		std::expected<void, std::string> Resize(uint32_t width, uint32_t height) final;

		VkSurfaceKHR	   GetSurface();
		VkSurfaceFormatKHR GetSurfaceFormat();

		void RecreateSwapchain();

		VkExtent2D GetSwapchainSize() const;

		bool			   IsSwapchainValid() const;
		const VkSemaphore &GetSemaphore();

	  private:
		void CreateSurface(VkInstance instance);
		bool CreateSwapchain(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		void CreateDepthStencil(GraphicsDeviceVk *graphicsDevice);
		void CreateResolveAttachment(GraphicsDeviceVk *graphicsDevice);
		void CreateSemaphores();

		void CreateFramebuffers();
		void CreateAll();

		void CleanupSwapchain();
		void CleanupSemaphores();

		bool AcquireNextImage();

		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void		CreateImage(uint32_t			  width,
								uint32_t			  height,
								VkFormat			  format,
								VkImageTiling		  tiling,
								VkImageUsageFlags	  usage,
								VkMemoryPropertyFlags properties,
								VkImage				 &image,
								VkDeviceMemory		 &imageMemory,
								VkSampleCountFlagBits samples,
								GraphicsDeviceVk	 *graphicsDevice);
		uint32_t	FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);
		uint32_t	GetCurrentFrameIndex();

	  private:
		CommandQueueVk *m_CommandQueue = nullptr;

		// vulkan types
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		PixelFormat m_DepthFormat = PixelFormat::D24_UNorm_S8_UInt;

		VkSwapchainKHR			 m_Swapchain		   = VK_NULL_HANDLE;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities = {};
		VkSurfaceFormatKHR		 m_SurfaceFormat	   = {};
		VkExtent2D				 m_SwapchainSize	   = {};

		std::vector<Ref<ITexture>>	   m_ColourAttachments = {};
		Ref<ITexture>				   m_DepthAttachment   = {};
		Ref<ITexture>				   m_ResolveAttachment = {};
		std::vector<Ref<IFramebuffer>> m_Framebuffers	   = {};

		GraphicsDeviceVk *m_GraphicsDevice;

		uint32_t m_FrameNumber		 = 0;
		uint32_t m_CurrentFrameIndex = 0;
		bool	 m_SwapchainValid	 = false;

		VkSemaphore m_PresentSemaphores[FRAMES_IN_FLIGHT];

		friend class GraphicsDeviceVk;
		friend class RenderPassVk;
		friend class CommandListVk;
		friend class CommandExecutorVk;
	};
}	 // namespace Nexus::Graphics

#endif