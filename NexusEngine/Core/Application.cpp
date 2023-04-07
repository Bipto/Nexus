#include "Application.h"
#include "SDL_opengl.h"

#include <iostream>

namespace Nexus
{
    Application::Application(GraphicsAPI api)
    {       

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "Could not initialize SDL\n";
        }

        WindowProperties props;
        this->m_Window = new Nexus::Window(props);

        Ref<Nexus::GraphicsDevice> device = Nexus::CreateGraphicsDevice(this->m_Window, api);
        this->m_GraphicsDevice = std::shared_ptr<GraphicsDevice>(device);
        this->m_GraphicsDevice->SetContext();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();            
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 

        if (device->GetGraphicsAPI() == GraphicsAPI::OpenGL)
        {
            SDL_Window* window = this->m_Window->GetSDLWindowHandle();
            SDL_GLContext context = (SDL_GLContext)this->m_GraphicsDevice->GetContext();

            ImGui_ImplSDL2_InitForOpenGL(window, context);
        }  

        else if (device->GetGraphicsAPI() == GraphicsAPI::DirectX11)
        {
            ImGui_ImplSDL2_InitForD3D(this->m_Window->GetSDLWindowHandle());
        }

        m_GraphicsDevice->InitialiseImGui();       
    }

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::BeginImGuiRender()
    {
        //ImGui_ImplOpenGL3_NewFrame();
        m_GraphicsDevice->BeginImGuiRender();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        m_ImGuiActive = true;
    }

    void Application::EndImGuiRender()
    {
        ImGui::Render();
        ImGui::GetMainViewport()->Size = { (float)this->GetWindowSize().Width, (float)this->GetWindowSize().Height };

        //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        m_GraphicsDevice->EndImGuiRender();
    }

    void Application::MainLoop()
    {
        if (m_ImGuiActive)
        {
            //Update and render additional platform windows
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }

        if (m_Window->m_RequiresResize)
        {
            OnResize(this->GetWindowSize());
            m_Window->m_RequiresResize = false;
        }

        this->m_Window->PollEvents(m_ImGuiActive);

        //Allow user to block closing events, for example to display save prompt
        if (m_Window->m_Closing)
            m_Window->m_Closing = this->OnClose();

        m_Clock.Tick();
        auto time = m_Clock.GetTime();
        this->Update(time);    
    }

    Point Application::GetWindowSize()
    {
        return this->m_Window->GetWindowSize();
    }

    Point Application::GetWindowPosition()
    {
        return this->m_Window->GetWindowPosition();
    }

    Window* Application::CreateApplicationWindow(const WindowProperties& props)
    {
        Window* window = new Nexus::Window(props);
        return window;
    }
}