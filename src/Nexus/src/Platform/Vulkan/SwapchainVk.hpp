#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "Nexus-Core/nxpch.hpp"

	#include "Nexus-Core/Graphics/Swapchain.hpp"
	#include "Nexus-Core/IWindow.hpp"
	#include "PhysicalDeviceVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	// forward declaration
	class GraphicsDeviceVk;
	class CommandQueueVk;

	class SwapchainVk : public Swapchain
	{
	  public:
		SwapchainVk(IWindow *window, GraphicsDevice *graphicsDevice, ICommandQueue *commandQueue, const SwapchainDescription &swapchainSpec);
		virtual ~SwapchainVk();

		void					 SwapBuffers() final;
		void					 SetPresentMode(PresentMode presentMode) final;
		Nexus::Point2D<uint32_t> GetSize() final;
		VkSurfaceKHR			 GetSurface();
		VkSurfaceFormatKHR		 GetSurfaceFormat();
		VkFormat				 GetVkDepthFormat();

		IWindow *GetWindow() final
		{
			return m_Window;
		}

		PixelFormat GetColourFormat() final;
		PixelFormat GetDepthFormat() final;

		void RecreateSwapchain();

		uint32_t   GetImageCount();
		VkExtent2D GetSwapchainSize() const;

		VkImage GetColourImage();
		VkImage GetDepthImage();
		VkImage GetResolveImage();

		VkImageView GetColourImageView();
		VkImageView GetDepthImageView();
		VkImageView GetResolveImageView();

		VkImageLayout GetColorImageLayout();
		VkImageLayout GetDepthImageLayout();
		VkImageLayout GetResolveImageLayout();

		void SetColorImageLayout(VkImageLayout layout);
		void SetDepthImageLayout(VkImageLayout layout);
		void SetResolveImageLayout(VkImageLayout layout);

		bool			   IsSwapchainValid() const;
		const VkSemaphore &GetSemaphore();
		VkRenderPass	   GetRenderPass() const;
		VkFramebuffer	   GetFramebuffer() const;

	  private:
		void CreateSurface(VkInstance instance);
		bool CreateSwapchain(std::shared_ptr<PhysicalDeviceVk> physicalDevice);
		void CreateSwapchainImageViews();
		void CreateDepthStencil(GraphicsDeviceVk *graphicsDevice);
		void CreateResolveAttachment(GraphicsDeviceVk *graphicsDevice);
		void CreateSemaphores();

		void CreateFramebuffers();
		void CreateRenderPass();

		void CreateAll();

		void CleanupSwapchain();
		void CleanupDepthStencil();
		void CleanupResolveAttachment();
		void CleanupSemaphores();
		void CleanupFramebuffers();

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
		IWindow		   *m_Window	   = nullptr;
		CommandQueueVk *m_CommandQueue = nullptr;

		// vulkan types
		VkSurfaceKHR m_Surface;

		VkSwapchainKHR			 m_Swapchain;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
		VkSurfaceFormatKHR		 m_SurfaceFormat;
		VkExtent2D				 m_SwapchainSize;

		std::vector<VkImage>	   m_SwapchainImages;
		uint32_t				   m_SwapchainImageCount;
		std::vector<VkImageView>   m_SwapchainImageViews;
		std::vector<VkImageLayout> m_ImageLayouts;

		VkImage		   m_ResolveImage;
		VkDeviceMemory m_ResolveMemory;
		VkImageView	   m_ResolveImageView;
		VkImageLayout  m_ResolveImageLayout;

		VkFormat	   m_DepthFormat = VK_FORMAT_D24_UNORM_S8_UINT;
		VkImage		   m_DepthImage;
		VkDeviceMemory m_DepthImageMemory;
		VkImageView	   m_DepthImageView;
		VkImageLayout  m_DepthLayout;

		std::vector<VkFramebuffer> m_Framebuffers;
		VkRenderPass			   m_RenderPass;

		GraphicsDeviceVk *m_GraphicsDevice;

		uint32_t m_FrameNumber		 = 0;
		uint32_t m_CurrentFrameIndex = 0;
		VkImage	 m_CurrentImage		 = nullptr;
		bool	 m_SwapchainValid	 = false;

		VkSemaphore m_PresentSemaphores[FRAMES_IN_FLIGHT];

		friend class GraphicsDeviceVk;
		friend class RenderPassVk;
		friend class CommandListVk;
		friend class CommandExecutorVk;
	};
}	 // namespace Nexus::Graphics

#endif