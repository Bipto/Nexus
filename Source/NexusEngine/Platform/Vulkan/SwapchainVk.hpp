#pragma once

#include "Core/Graphics/Swapchain.hpp"
#include "Core/Window.hpp"
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
        virtual void Resize(uint32_t width, uint32_t height) override;

    private:
        Window *m_Window;
        VSyncState m_VsyncState;

        // vulkan types
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

        friend class GraphicsDeviceVk;
        friend class RenderPassVk;
    };
}