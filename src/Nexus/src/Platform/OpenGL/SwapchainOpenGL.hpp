#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Window.hpp"

namespace Nexus::Graphics
{
    class SwapchainOpenGL : public Swapchain
    {
    public:
        SwapchainOpenGL(Window *window, const SwapchainSpecification &swapchainSpec);
        virtual ~SwapchainOpenGL();
        virtual void Initialise() override {}
        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        void ResizeIfNecessary();
        void BindAsRenderTarget();
        void BindAsDrawTarget();

        virtual Window *GetWindow() override { return m_Window; }
        virtual void Prepare() override;

        static bool HasContextBeenCreated();

    private:
        Window *m_Window;
        VSyncState m_VsyncState;

        uint32_t m_SwapchainWidth = 0;
        uint32_t m_SwapchainHeight = 0;

        SDL_GLContext m_Context = nullptr;
        int m_Backbuffer = 0;

        static SDL_Window *s_ContextWindow;
    };
}

#endif