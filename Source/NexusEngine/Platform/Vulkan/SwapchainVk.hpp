#pragma once

#if defined(NX_PLATFORM_VULKAN)

#include "Core/Graphics/Swapchain.hpp"
#include "Core/Window.hpp"
#include "SDL_vulkan.h"
#include "Vk.hpp"

namespace Nexus::Graphics
{
    // forward declaration
    class GraphicsDeviceVk;

    class SwapchainVk : public Swapchain
    {
    public:
        SwapchainVk(Window *window, VSyncState vSyncState, GraphicsDeviceVk *graphicsDevice);
        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        const VkFramebuffer &GetCurrentFramebuffer();
        VkSurfaceFormatKHR GetSurfaceFormat();
        VkFormat GetDepthFormat();

    private:
        void CreateSurface();
        void CreateSwapchain();
        void CreateSwapchainImageViews();
        void CreateDepthStencil();
        void CreateFramebuffers();

        void CleanupSwapchain();
        void CleanupDepthStencil();

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

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

        VkFormat m_DepthFormat;
        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;

        std::vector<VkFramebuffer> m_SwapchainFramebuffers;
        GraphicsDeviceVk *m_GraphicsDevice;

        VkPresentModeKHR m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;

        friend class GraphicsDeviceVk;
        friend class RenderPassVk;
        friend class CommandListVk;
        friend class ImGuiRenderer;
    };
}

#endif