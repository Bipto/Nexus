#pragma once

#include "Nexus/Graphics/Swapchain.hpp"
#include "Nexus/Window.hpp"

namespace Nexus::Graphics
{
    class SwapchainOpenGL : public Swapchain
    {
    public:
        SwapchainOpenGL(Window *window, VSyncState vSyncState);
        virtual ~SwapchainOpenGL();
        virtual void Initialise() override {}
        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        void ResizeIfNecessary();
        void Bind();

        virtual Window *GetWindow() override { return m_Window; }

        static bool HasContextBeenCreated();

    private:
        Window *m_Window;
        VSyncState m_VsyncState;

        uint32_t m_SwapchainWidth = 0;
        uint32_t m_SwapchainHeight = 0;

        SDL_GLContext m_Context = nullptr;
        int m_Backbuffer = 0;

        static bool s_ContextCreated;
        static SDL_Window *s_ContextWindow;
    };
}