#pragma once

#include "SDL.h"
#include "Core/Size.h"
#include "GraphicsDevice.h"

namespace Nexus
{
    enum class VSyncState
    {
        Adaptive = -1,
        Disabled = 0,
        Enabled = 1
    };

    class Swapchain
    {
        public:
            Swapchain(SDL_Window* window, Size size)
            {
                this->m_Window = window;
                this->m_Size = size;
            }
            Swapchain(const Swapchain&) = delete;
                     
            virtual void Resize(Size size) = 0;
            virtual void Present() = 0;
            virtual void SetVSyncState(VSyncState vSyncState) = 0;

        protected:
            SDL_Window* m_Window;
            Size m_Size;
    };
}