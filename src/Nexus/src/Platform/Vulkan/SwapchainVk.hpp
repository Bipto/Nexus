#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Window.hpp"
#include "SDL_vulkan.h"
#include "Vk.hpp"

namespace Nexus::Graphics
{
// forward declaration
class GraphicsDeviceVk;

class SwapchainVk : public Swapchain
{
  public:
    SwapchainVk(Window *window, GraphicsDevice *graphicsDevice, const SwapchainSpecification &swapchainSpec);
    virtual ~SwapchainVk();

    virtual void SwapBuffers() override;
    virtual VSyncState GetVsyncState() override;
    virtual void SetVSyncState(VSyncState vsyncState) override;
    VkSurfaceFormatKHR GetSurfaceFormat();
    VkFormat GetDepthFormat();

    virtual Window *GetWindow() override
    {
        return m_Window;
    }
    virtual void Prepare() override;

    virtual void Initialise() override;
    void RecreateSwapchain();

    uint32_t GetImageCount();

    VkImage GetColourImage();
    VkImage GetDepthImage();

    VkImageView GetColourImageView();
    VkImageView GetDepthImageView();

    VkImageLayout GetColorImageLayout();
    VkImageLayout GetDepthImageLayout();
    void SetColorImageLayout(VkImageLayout layout);
    void SetDepthImageLayout(VkImageLayout layout);

    bool IsSwapchainValid() const;
    const VkSemaphore &GetSemaphore();

  private:
    void CreateSurface();
    bool CreateSwapchain();
    void CreateSwapchainImageViews();
    void CreateDepthStencil();
    void CreateResolveAttachment();
    void CreateSemaphores();

    void CleanupSwapchain();
    void CleanupDepthStencil();
    void CleanupResolveAttachment();
    void CleanupSemaphores();

    bool AcquireNextImage();

    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                     VkDeviceMemory &imageMemory, VkSampleCountFlagBits samples);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    uint32_t GetCurrentFrameIndex();

  private:
    Window *m_Window;
    VSyncState m_VsyncState;

    // vulkan types
    VkSurfaceKHR m_Surface;

    VkSwapchainKHR m_Swapchain;
    VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
    VkSurfaceFormatKHR m_SurfaceFormat;
    VkExtent2D m_SwapchainSize;

    std::vector<VkImage> m_SwapchainImages;
    uint32_t m_SwapchainImageCount;
    std::vector<VkImageView> m_SwapchainImageViews;
    std::vector<VkImageLayout> m_ImageLayouts;

    VkImage m_ResolveImage;
    VkDeviceMemory m_ResolveMemory;
    VkImageView m_ResolveImageView;

    VkFormat m_DepthFormat;
    VkImage m_DepthImage;
    VkDeviceMemory m_DepthImageMemory;
    VkImageView m_DepthImageView;
    VkImageLayout m_DepthLayout;

    GraphicsDeviceVk *m_GraphicsDevice;

    uint32_t m_FrameNumber = 0;
    uint32_t m_CurrentFrameIndex = 0;
    VkImage m_CurrentImage = nullptr;
    bool m_SwapchainValid = false;

    VkSemaphore m_PresentSemaphores[FRAMES_IN_FLIGHT];

    friend class GraphicsDeviceVk;
    friend class RenderPassVk;
    friend class CommandListVk;
    friend class CommandExecutorVk;
};
} // namespace Nexus::Graphics

#endif