#pragma once

#include "Nexus/Graphics/Swapchain.hpp"
#include "SDL.h"

namespace Nexus::Graphics
{
    class SwapchainOpenGL : public Swapchain
    {
    public:
        SwapchainOpenGL(SDL_Window *window, VSyncState vSyncState);
        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        void Resize(uint32_t width, uint32_t height);

    private:
        SDL_Window *m_Window;
        VSyncState m_VsyncState;
    };
}