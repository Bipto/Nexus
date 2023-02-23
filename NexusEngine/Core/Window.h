#pragma once

#include "nxpch.h"
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Point.h"
#include "Core/Events/Event.h"
#include "Core/Events/EventHandler.h"
#include "Core/Input/Input.h"

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
            Window(const WindowProperties& windowProps);
            Window(const Window&) = delete;
            ~Window();

            void PollEvents();

            void SetResizable(bool isResizable);
            void SetTitle(const std::string& title);
            void SetSize(Point size);

            void Close() { m_Closing = true; }
            bool IsClosing() {return this->m_Closing;}

            SDL_Window* GetSDLWindowHandle(){return this->m_Window;}
            Point GetWindowSize();

            Input* GetInput() { return m_Input; }

        private:
            SDL_Window* m_Window;
            bool m_Closing = false;
            Input* m_Input;
            friend class Application;
    };
}