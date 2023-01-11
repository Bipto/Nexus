#include "Window.h"
#include "glad/glad.h"

namespace Nexus
{
    Window::Window(const WindowProperties& windowProps)
    {   
        this->m_Window = SDL_CreateWindow(windowProps.Title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowProps.Width, windowProps.Height, SDL_WINDOW_OPENGL);
        SetResizable(windowProps.Resizable);
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
            ImGui_ImplSDL2_ProcessEvent(&event);
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

    void Window::SetSize(Size size)
    {
        SDL_SetWindowSize(this->m_Window, size.Width, size.Height);
    }

    Size Window::GetWindowSize()
    {
        Size size{};
        SDL_GetWindowSize(this->m_Window, &size.Width, &size.Height);
        return size;
    }
}