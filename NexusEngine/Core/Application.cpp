#include "Application.h"

namespace NexusEngine
{
    Application::Application(GraphicsAPI api)
    {
        this->m_Window = new NexusEngine::Window("My Window", {960, 650});
        this->m_Window->SetResizable(true);
        this->m_Window->SetTitle("Different app name");
        this->m_Window->SetSize({640, 480});

        NexusEngine::GraphicsDevice* device = NexusEngine::CreateGraphicsDevice(this->m_Window->GetSDLWindowHandle(), api);
        this->m_GraphicsDevice = std::shared_ptr<GraphicsDevice>(device);
        this->m_GraphicsDevice->SetContext();
    }

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::MainLoop()
    {
        /* while (!this->m_Window->IsClosing())
        {
            this->m_Window->PollEvents();
        } */

        this->m_Window->PollEvents();
        this->Update();
    }
}