#pragma once

#include "../../Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"

#include <iostream>

namespace NexusEngine
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
        public:
            GraphicsDeviceOpenGL(SDL_Window* window, GraphicsAPI api) : GraphicsDevice(window, api)
            {
                this->m_Context = SDL_GL_CreateContext(this->m_Window);

                if (this->m_Context == NULL)
                {
                    std::cout << SDL_GetError() << std::endl;
                }

                #ifndef __EMSCRIPTEN__
                    gladLoadGL();
                #endif
            }

            GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL&) = delete;

            void SetContext() override 
            {
                SDL_GL_MakeCurrent(this->m_Window, this->m_Context);
            }

            void Clear(float red, float green, float blue, float alpha) override 
            {
                glClearColor(red, green, blue, alpha);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            void Resize(int width, int height) override
            {
                glViewport(0, 0, width, height);
            }

            void SwapBuffers() override
            {
                SDL_GL_SwapWindow(this->m_Window);
            }

            void DrawElements(unsigned int start, unsigned int count) override {}
            void SetVSync(VSyncState vSyncState) override {}

        private:
            SDL_GLContext m_Context;
    };
}