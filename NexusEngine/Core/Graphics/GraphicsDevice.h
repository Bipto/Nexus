#pragma once

#include "SDL.h"

namespace NexusEngine
{
    enum class GraphicsAPI
    {
        None,
        OpenGL,
        DirectX11
    };

    enum class VSyncState
    {
        Adaptive = -1,
        Disabled = 0,
        Enabled = 1
    };

    class GraphicsDevice
    {
        public:
            GraphicsDevice(SDL_Window* window, GraphicsAPI api)
            {
                this->m_Window = window;
                this->m_API = api;
            }
            GraphicsDevice(const GraphicsDevice&) = delete;
    
            virtual void SetContext() = 0;
            virtual void Clear(float red, float green, float blue, float alpha) = 0;
            virtual void Resize(int width, int height) = 0;
            virtual void SwapBuffers() = 0;
            virtual void DrawElements(unsigned int start, unsigned int count) = 0;
            virtual void SetVSync(VSyncState vSyncState) = 0;

            GraphicsAPI GetGraphicsAPI(){return this->m_API;}

        protected:
            SDL_Window* m_Window;
            GraphicsAPI m_API;
    };
}