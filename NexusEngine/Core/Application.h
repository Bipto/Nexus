#pragma once

#include "nxpch.h"

#include "Window.h"
#include "Graphics/GraphicsDevice.h"
#include "Platform/OpenGL/GraphicsDeviceOpenGL.h"
#include "Platform/DX11/GraphicsDeviceDX11.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Point.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "backends/imgui_impl_sdl2.h"

#include "Core/Events/Event.h"
#include "Core/Events/EventHandler.h"
#include "Core/Input/Input.h"
#include "Core/Time.h"
#include "Core/Memory.h"

#include "Runtime/ECS/ComponentRegistry.h"

#include <chrono>

class Clock
{
    public:
        void Tick()
        {
            std::chrono::steady_clock::time_point tickTime = std::chrono::steady_clock::now();
            m_DeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(tickTime - m_StartTime).count();
            m_StartTime = tickTime;
        }

        Nexus::Time GetTime()
        {
            return Nexus::Time(m_DeltaTime);
        }
    private:
        std::chrono::steady_clock::time_point m_StartTime = std::chrono::steady_clock::now();
        uint64_t m_DeltaTime;
};

namespace Nexus
{
    static Ref<GraphicsDevice> CreateGraphicsDevice(Nexus::Window* window, GraphicsAPI api)
    {
        Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = window->GetWindowSize().Width;
        vp.Height = window->GetWindowSize().Height;

        switch (api)
        {
            case GraphicsAPI::DirectX11:
                return CreateRef<GraphicsDeviceDX11>(window, api, vp);
            default:
                return CreateRef<GraphicsDeviceOpenGL>(window, api, vp);
        }
    }

    class Application
    {
        public:
            Application(GraphicsAPI api);
            Application(const Application&) = delete;
            ~Application();

            //required overridable methods
            virtual void Load() = 0;
            virtual void Update(Nexus::Time time) = 0;
            virtual void Unload() = 0;

            //optional overridable methods
            virtual void OnResize(Point size) {}
            virtual bool OnClose() { return true; }

            void BeginImGuiRender();
            void EndImGuiRender();

            void MainLoop();

            Point GetWindowSize();
            Point GetWindowPosition();

            void Close(){ this->m_Window->Close(); }
            bool ShouldClose(){return this->m_Window->IsClosing();}

            Window* CreateApplicationWindow(const WindowProperties& props);
            Input* GetCoreInput() { return m_Window->GetInput(); }
            
        protected:
            Ref<GraphicsDevice> m_GraphicsDevice;

        private:
            Nexus::Window* m_Window;
            Point m_PreviousWindowSize;

            Nexus::EventHandler<Point> m_WindowResizeEventHandler;
            Clock m_Clock;
            bool m_ImGuiActive = false;
    };
}