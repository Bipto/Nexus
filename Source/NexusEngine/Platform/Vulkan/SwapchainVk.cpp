#include "SwapchainVk.hpp"

#include "GraphicsDeviceVk.hpp"

namespace Nexus::Graphics
{
    SwapchainVk::SwapchainVk(Window *window, VSyncState vSyncState, GraphicsDeviceVk *graphicsDevice)
    {
        m_Window = window;
        m_VsyncState = vSyncState;
        m_GraphicsDevice = graphicsDevice;
    }

    void SwapchainVk::SwapBuffers()
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_GraphicsDevice->GetCurrentFrame().PresentSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_GraphicsDevice->m_CurrentFrameIndex;

        VkResult result = vkQueuePresentKHR(m_GraphicsDevice->m_PresentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            m_GraphicsDevice->RecreateSwapchain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swapchain image");
        }

        if (vkQueueWaitIdle(m_GraphicsDevice->m_PresentQueue) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to wait for present queue");
        }

        m_GraphicsDevice->m_FrameNumber++;
    }

    VSyncState SwapchainVk::GetVsyncState()
    {
        return VSyncState();
    }

    void SwapchainVk::SetVSyncState(VSyncState vsyncState)
    {
    }

    void SwapchainVk::Resize(uint32_t width, uint32_t height)
    {
    }
}
