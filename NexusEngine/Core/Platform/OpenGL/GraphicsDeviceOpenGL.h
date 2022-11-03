#pragma once

#include "../../Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"

namespace NexusEngine
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
        public:
            GraphicsDeviceOpenGL(SDL_Window* window, GraphicsAPI api) : GraphicsDevice(window, api)
            {
                // Decide GL+GLSL versions
                #ifdef __EMSCRIPTEN__
                    // GL ES 2.0 + GLSL 100
                    const char* glsl_version = "#version 100";
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
                #elif defined(__APPLE__)
                    // GL 3.2 Core + GLSL 150
                    const char* glsl_version = "#version 150";
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
                #else
                    // GL 3.0 + GLSL 130
                    const char* glsl_version = "#version 130";
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
                #endif

                    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
                    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

                this->m_Context = SDL_GL_CreateContext(this->m_Window);
            }

            GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL&) = delete;

            void SetContext() override {}
            void Clear(float red, float green, float blue, float alpha) override {}
            void Resize(int width, int height) override {}
            void Present() override {}
            void DrawElements(unsigned int start, unsigned int count) override {}
            void SetVSync(VSyncState vSyncState) override {}

        private:
            SDL_GLContext m_Context;
    };
}