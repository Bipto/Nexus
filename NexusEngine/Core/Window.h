#pragma once

#include <string>
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Size.h"

namespace NexusEngine
{
    enum WindowState
    {
        Normal,
        Minimized,
        Maximized
    };

    class Window
    {
        public:
            Window(const std::string& title, Size size);
            Window(const Window&) = delete;
            ~Window();

            void PollEvents();
            bool IsClosing() {return this->m_Closing;}

            void SetResizable(bool isResizable);
            void SetTitle(const std::string& title);
            void SetSize(Size size);

            SDL_Window* GetSDLWindowHandle(){return this->m_Window;}
            Size GetWindowSize();

        private:
            SDL_Window* m_Window;
            bool m_Closing = false;
    };
}