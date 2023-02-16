#include "Application.h"
#include "SDL_opengl.h"

namespace Nexus
{
    Application::Application(GraphicsAPI api)
    {
        gladLoadGL();

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

        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "Could not initialize SDL\n";
        }

        WindowProperties props;
        this->m_Window = new Nexus::Window(props);

        Nexus::GraphicsDevice* device = Nexus::CreateGraphicsDevice(this->m_Window, api);
        this->m_GraphicsDevice = std::shared_ptr<GraphicsDevice>(device);
        this->m_GraphicsDevice->SetContext();

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

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::BeginImGuiRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void Application::EndImGuiRender()
    {
        ImGui::Render();
        /* this->m_GraphicsDevice->Resize(this->GetWindowSize());
        this->m_GraphicsDevice->Clear(0, 0, 0, 0); */

        ImGui::GetMainViewport()->Size = { (float)this->GetWindowSize().Width, (float)this->GetWindowSize().Height };

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Application::MainLoop()
    {
        auto windowSize = this->GetWindowSize();
        if (m_PreviousWindowSize.Width != windowSize.Width || m_PreviousWindowSize.Height != windowSize.Height)
        {
            OnResize(windowSize);
            m_PreviousWindowSize = windowSize;
        }

        if (m_Window->GetShouldWindowClose())
        {
            if (OnClose())
            {
                m_Window->Close();
            }
            else
            {
                m_Window->SetShouldWindowClose(false);
            }
        }

        this->m_Window->PollEvents();

        m_Clock.Tick();
        auto time = m_Clock.GetTime();
        this->Update(time);    

        //Update and render additional platform windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    Point Application::GetWindowSize()
    {
        return this->m_Window->GetWindowSize();
    }

    Window* Application::CreateWindow(const WindowProperties& props)
    {
        Window* window = new Nexus::Window(props);
        return window;
    }
}