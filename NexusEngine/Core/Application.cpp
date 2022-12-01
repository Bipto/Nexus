#include "Application.h"

namespace Nexus
{
    Application::Application(GraphicsAPI api)
    {
        
        this->m_Window = new Nexus::Window("My Window", {960, 650});
        this->m_Window->SetResizable(true);

        Nexus::GraphicsDevice* device = Nexus::CreateGraphicsDevice(this->m_Window, api);
        this->m_GraphicsDevice = std::shared_ptr<GraphicsDevice>(device);
        this->m_GraphicsDevice->SetContext();
    }

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::MainLoop()
    {
        this->m_Window->PollEvents();
        this->Update();
    }

    Size Application::GetWindowSize()
    {
        return this->m_Window->GetWindowSize();
    }
}