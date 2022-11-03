#include "Application.h"

namespace NexusEngine
{
    Application::Application()
    {
        this->m_Window = new NexusEngine::Window("My Window", {960, 650});
        this->m_Window->SetResizable(true);
        this->m_Window->SetTitle("Different app name");
        this->m_Window->SetSize({640, 480});
    }

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::Run()
    {
        while (!this->m_Window->IsClosing())
        {
            this->m_Window->PollEvents();
        }
    }
}