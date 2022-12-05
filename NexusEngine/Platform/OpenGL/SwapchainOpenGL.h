#pragma once

#include "Core/Graphics/Swapchain.h"
#include "GL.h"
#include "SDL_opengl.h"

namespace Nexus
{
    class SwapchainOpenGL : public Swapchain
    {
        public:
            SwapchainOpenGL(SDL_Window* window, Size size) : Swapchain(window, size){}

            void Resize(Size size) override
            {
                this->m_Size = size;
                glViewport(0, 0, size.Width, size.Height);
            }

            void Present() override
            {
                SDL_GL_SwapWindow(this->m_Window);
            }

            void SetVSyncState(VSyncState vSyncState) override
            {
                SDL_GL_SetSwapInterval((unsigned int)vSyncState);
            }
    };
}