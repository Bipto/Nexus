#pragma once

#include "../../Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"
#include "SwapchainOpenGL.h"

#include <iostream>

namespace NexusEngine
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
        public:
            GraphicsDeviceOpenGL(NexusEngine::Window* window, GraphicsAPI api) : GraphicsDevice(window, api)
            {
                this->m_Context = SDL_GL_CreateContext(this->m_Window->GetSDLWindowHandle());

                if (this->m_Context == NULL)
                {
                    std::cout << SDL_GetError() << std::endl;
                }

                #ifndef __EMSCRIPTEN__
                    gladLoadGL();
                #endif

                this->m_Swapchain = new SwapchainOpenGL(this->m_Window->GetSDLWindowHandle(), this->m_Window->GetWindowSize());
            }

            GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL&) = delete;

            void SetContext() override 
            {
                SDL_GL_MakeCurrent(this->m_Window->GetSDLWindowHandle(), this->m_Context);
            }

            void Clear(float red, float green, float blue, float alpha) override 
            {
                glClearColor(red, green, blue, alpha);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            void DrawElements(unsigned int start, unsigned int count) override {}

        private:
            SDL_GLContext m_Context;
    };
}