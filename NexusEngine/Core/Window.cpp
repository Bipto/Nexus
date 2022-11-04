#include "Window.h"
#include "glad/glad.h"

namespace NexusEngine
{
    Window::Window(const std::string& title, WindowSize size)
    {   
        this->m_Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.Width, size.Height, SDL_WINDOW_OPENGL);
    }   

    Window::~Window()
    {
        SDL_DestroyWindow(this->m_Window);
    }

    void Window::PollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    this->m_Closing = true;
                    break;
            }
        }
    }

    void Window::SetResizable(bool isResizable)
    {
        SDL_SetWindowResizable(this->m_Window, (SDL_bool)isResizable);
    }

    void Window::SetTitle(const std::string& title)
    {
        SDL_SetWindowTitle(this->m_Window, title.c_str());
    }

    void Window::SetSize(WindowSize size)
    {
        SDL_SetWindowSize(this->m_Window, size.Width, size.Height);
    }

    WindowSize Window::GetWindowSize()
    {
        WindowSize size{};
        SDL_GetWindowSize(this->m_Window, &size.Width, &size.Height);
        return size;
    }
}