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
#include "Core/Input/InputState.h"
#include "Core/Time.h"
#include "Core/Memory.h"

#include "Runtime/ECS/ComponentRegistry.h"

#include <chrono>

class Clock
{
public:
    void Tick()
    {
        std::chrono::high_resolution_clock::time_point tickTime = std::chrono::high_resolution_clock::now();
        m_DeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(tickTime - m_StartTime).count();
        m_StartTime = tickTime;
    }

    Nexus::Time GetTime()
    {
        return Nexus::Time(m_DeltaTime);
    }

private:
    std::chrono::high_resolution_clock::time_point m_StartTime = std::chrono::high_resolution_clock::now();
    uint64_t m_DeltaTime;
};

namespace Nexus
{
    static Ref<GraphicsDevice> CreateGraphicsDevice(const GraphicsDeviceCreateInfo &createInfo)
    {
        switch (createInfo.API)
        {
        case GraphicsAPI::DirectX11:
            return CreateRef<GraphicsDeviceDX11>(createInfo);
        default:
            return CreateRef<GraphicsDeviceOpenGL>(createInfo);
        }
    }

    struct ApplicationSpecification
    {
        GraphicsAPI API;
        uint32_t UpdatesPerSecond = 60;
        uint32_t RendersPerSecond = 60;
        VSyncState VSyncState = VSyncState::Enabled;
        bool ImGuiActive = false;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification &spec);
        Application(const Application &) = delete;
        ~Application();

        // required overridable methods
        virtual void Load() = 0;
        virtual void Update(Nexus::Time time) = 0;
        virtual void Render(Nexus::Time time) = 0;
        virtual void Unload() = 0;

        // optional overridable methods
        virtual void OnResize(Point<int> size) {}
        virtual bool OnClose() { return true; }

        void MainLoop();

        Point<int> GetWindowSize();
        Point<int> GetWindowPosition();

        void Close() { this->m_Window->Close(); }
        bool ShouldClose() { return this->m_Window->IsClosing(); }

        Window *CreateApplicationWindow(const WindowProperties &props);
        InputState *GetCoreInputState() { return m_Window->GetInput(); }

        Ref<GraphicsDevice> GetGraphicsDevice() { return m_GraphicsDevice; }

    protected:
        Ref<GraphicsDevice> m_GraphicsDevice;

    private:
        ApplicationSpecification m_Specification;
        Nexus::Window *m_Window;
        Point<int> m_PreviousWindowSize;

        Nexus::EventHandler<Point<int>> m_WindowResizeEventHandler;
        Clock m_Clock;
        double m_UpdateTimer = 0;
        double m_RenderTimer = 0;

        Nexus::Time m_PreviousUpdateTime;
        Nexus::Time m_PreviousRenderTime;
    };
}