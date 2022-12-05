#pragma once

#include "nxpch.h"

#include "Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Platform/OpenGL/GraphicsDeviceOpenGL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Size.h"

namespace Nexus
{
    static GraphicsDevice* CreateGraphicsDevice(Nexus::Window* window, GraphicsAPI api)
    {
        switch (api)
        {
            default:
                return new GraphicsDeviceOpenGL(window, api);
        }
    }

    class Application
    {
        public:
            Application(GraphicsAPI api);
            Application(const Application&) = delete;
            ~Application();

            //overridable methods
            virtual void Load() = 0;
            virtual void Update() = 0;
            virtual void Unload() = 0;

            void MainLoop();

            Size GetWindowSize();
            bool ShouldClose(){return this->m_Window->IsClosing();}

            Window* CreateWindow(const WindowProperties& props);

        protected:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;

        private:
            Nexus::Window* m_Window;
    };
}