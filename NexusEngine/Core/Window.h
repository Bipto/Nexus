#pragma once

#include <string>
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace NexusEngine
{
    enum WindowState
    {
        Normal,
        Minimized,
        Maximized
    };

    struct WindowSize
    {
        int Width, Height;
    };

    class Window
    {
        public:
            Window(const std::string& title, WindowSize size);
            Window(const Window&) = delete;
            ~Window();

            void PollEvents();
            bool IsClosing() {return this->m_Closing;}

            void SetResizable(bool isResizable);
            void SetTitle(const std::string& title);
            void SetSize(WindowSize size);

            SDL_Window* GetSDLWindowHandle(){return this->m_Window;}
            WindowSize GetWindowSize();

        private:
            SDL_Window* m_Window;
            bool m_Closing = false;
    };
}