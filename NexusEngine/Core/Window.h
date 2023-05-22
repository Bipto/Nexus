#pragma once

#include "nxpch.h"
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "Point.h"
#include "Core/Events/Event.h"
#include "Core/Events/EventHandler.h"
#include "Core/Input/InputState.h"

#include "backends/imgui_impl_sdl2.h"

namespace Nexus
{
    enum WindowState
    {
        Normal,
        Minimized,
        Maximized
    };

    struct WindowProperties
    {
        std::string Title = "My Window";
        int Width = 1280;
        int Height = 720;
        bool Resizable = true;
    };

    class Window
    {
    public:
        Window(const WindowProperties &windowProps);
        Window(const Window &) = delete;
        ~Window();

        void PollEvents(bool imguiActive = false);

        void SetResizable(bool isResizable);
        void SetTitle(const std::string &title);
        void SetSize(Point<int> size);

        void Close() { m_Closing = true; }
        bool IsClosing() { return this->m_Closing; }

        SDL_Window *GetSDLWindowHandle() { return this->m_Window; }
        Point<int> GetWindowSize();
        Point<int> GetWindowPosition();

        InputState *GetInput() { return m_Input; }

    private:
        SDL_Window *m_Window;
        bool m_Closing = false;
        bool m_RequiresResize = false;
        InputState *m_Input;
        friend class Application;
    };
}