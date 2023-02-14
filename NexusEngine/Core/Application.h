#pragma once

#include "nxpch.h"

#include "Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Platform/OpenGL/GraphicsDeviceOpenGL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Point.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"

#include "Core/Events/Event.h"
#include "Core/Events/EventHandler.h"
#include "Core/Input/Input.h"

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

            virtual void OnResize(Point size) {}

            virtual bool OnClose() { return true; }

            void BeginImGuiRender();
            void EndImGuiRender();

            void MainLoop();

            Point GetWindowSize();
            void Close(){ this->m_Window->SetShouldWindowClose(true); }
            bool ShouldClose(){return this->m_Window->IsClosing();}

            Window* CreateWindow(const WindowProperties& props);

            Input* GetCoreInput() { return m_Window->GetInput(); }

        protected:
            std::shared_ptr<GraphicsDevice> m_GraphicsDevice;

        private:
            Nexus::Window* m_Window;
            Point m_PreviousWindowSize;

            Nexus::EventHandler<Point> m_WindowResizeEventHandler;
    };
}