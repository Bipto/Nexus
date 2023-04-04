#include "Application.h"
#include "SDL_opengl.h"

#include <iostream>

namespace Nexus
{
    Application::Application(GraphicsAPI api)
    {
        // Decide GL+GLSL versions
        #ifdef __EMSCRIPTEN__
            // GL ES 2.0 + GLSL 100
            const char* glsl_version = "#version 100";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #elif defined(__APPLE__)
            // GL 3.2 Core + GLSL 150
            const char* glsl_version = "#version 150";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #else
            // GL 3.0 + GLSL 130
            const char* glsl_version = "#version 130";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        #endif

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        #ifndef __EMSCRIPTEN__
            gladLoadGL();
        #endif

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "Could not initialize SDL\n";
        }

        WindowProperties props;
        this->m_Window = new Nexus::Window(props);

        Ref<Nexus::GraphicsDevice> device = Nexus::CreateGraphicsDevice(this->m_Window, api);
        this->m_GraphicsDevice = std::shared_ptr<GraphicsDevice>(device);
        this->m_GraphicsDevice->SetContext();

        if (device->GetGraphicsAPI() == GraphicsAPI::OpenGL)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            
            ImGui::StyleColorsDark();
            SDL_Window* window = this->m_Window->GetSDLWindowHandle();
            SDL_GLContext context = (SDL_GLContext)this->m_GraphicsDevice->GetContext();

            ImGui_ImplSDL2_InitForOpenGL(window, context);
            ImGui_ImplOpenGL3_Init(glsl_version);
        }  
        
    }

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::BeginImGuiRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        m_ImGuiActive = true;
    }

    void Application::EndImGuiRender()
    {
        ImGui::Render();
        ImGui::GetMainViewport()->Size = { (float)this->GetWindowSize().Width, (float)this->GetWindowSize().Height };

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Application::MainLoop()
    {
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

        if (m_ImGuiActive)
        {
            //Update and render additional platform windows
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }
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