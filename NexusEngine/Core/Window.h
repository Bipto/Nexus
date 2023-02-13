#pragma once

#include "nxpch.h"
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Size.h"

#include "backends/imgui_impl_sdl.h"

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
            bool IsClosing() {return this->m_Closing;}

            void SetResizable(bool isResizable);
            void SetTitle(const std::string& title);
            void SetSize(Size size);
            void Close() { m_Closing = true; }

            SDL_Window* GetSDLWindowHandle(){return this->m_Window;}
            Size GetWindowSize();

        private:
            SDL_Window* m_Window;
            bool m_Closing = false;
    };
}