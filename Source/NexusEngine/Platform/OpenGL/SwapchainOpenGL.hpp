#pragma once

#include "Nexus/Graphics/Swapchain.hpp"
#include "Nexus/Window.hpp"

namespace Nexus::Graphics
{
    class SwapchainOpenGL : public Swapchain
    {
    public:
        SwapchainOpenGL(Window *window, VSyncState vSyncState);
        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        void ResizeIfNecessary();

    private:
        Window *m_Window;
        VSyncState m_VsyncState;

        uint32_t m_SwapchainWidth = 0;
        uint32_t m_SwapchainHeight = 0;
    };
}