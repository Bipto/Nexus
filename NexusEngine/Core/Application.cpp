#include "Application.h"
#include "SDL_opengl.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    Application::Application(const ApplicationSpecification& spec)
    {       
        m_Specification = spec;

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
        {
            NX_LOG("Could not initialize SDL");
        }

        WindowProperties props;
        this->m_Window = new Nexus::Window(props);

        GraphicsDeviceCreateInfo graphicsDeviceCreateInfo;
        graphicsDeviceCreateInfo.GraphicsWindow = m_Window;
        graphicsDeviceCreateInfo.API = spec.API;
        graphicsDeviceCreateInfo.VSyncStateSettings = spec.VSyncState;

        Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = m_Window->GetWindowSize().X;
        vp.Height = m_Window->GetWindowSize().Y;
        graphicsDeviceCreateInfo.GraphicsViewport = vp;

        Ref<Nexus::GraphicsDevice> device = Nexus::CreateGraphicsDevice(graphicsDeviceCreateInfo);
        m_GraphicsDevice = std::shared_ptr<GraphicsDevice>(device);
        m_GraphicsDevice->SetContext();
        m_GraphicsDevice->Resize(GetWindowSize());
        m_GraphicsDevice->SetVSyncState(spec.VSyncState);

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

        int controllerCount = SDL_NumJoysticks();
        for (int i = 0; i < controllerCount; i++)
        {
            m_Window->m_Input->ConnectController(i);
        }
    }

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::MainLoop()
    {
        if (m_Window->m_RequiresResize)
        {
            OnResize(m_PreviousWindowSize);
            m_Window->m_RequiresResize = false;
            m_PreviousWindowSize = this->GetWindowSize();
        }

        this->m_Window->PollEvents(m_Specification.ImGuiActive);

        //Allow user to block closing events, for example to display save prompt
        if (m_Window->m_Closing)
            m_Window->m_Closing = this->OnClose();

        m_Clock.Tick();
        auto time = m_Clock.GetTime();
        m_UpdateTimer += time.GetMilliseconds();
        m_RenderTimer += time.GetMilliseconds(); 

        double timeBetweenUpdates = (1000 / m_Specification.UpdatesPerSecond);
        double timeBetweenRenders = (1000 / m_Specification.RendersPerSecond);

        if (m_UpdateTimer > timeBetweenUpdates)
        {
            this->Update(time);   
            m_UpdateTimer = 0;
        }

        //if vsync is disabled, check if we should render yet
        if (m_GraphicsDevice->GetVsyncState() == Nexus::VSyncState::Disabled)
            if (m_RenderTimer < timeBetweenRenders)
                return;

        //run render functions
        {
            if (m_Specification.ImGuiActive)
            {
                m_GraphicsDevice->BeginImGuiRender();
                ImGui_ImplSDL2_NewFrame();
                ImGui::NewFrame();
            }           

            this->Render(time);
            m_RenderTimer = 0;

            if (m_Specification.ImGuiActive)
            {
                Nexus::Viewport vp;
                vp.X = 0;
                vp.Y = 0;
                vp.Width = m_Window->GetWindowSize().X;
                vp.Height = m_Window->GetWindowSize().Y;
                m_GraphicsDevice->SetViewport(vp);

                ImGui::Render();
                ImGui::GetMainViewport()->Size = { (float)this->GetWindowSize().X, (float)this->GetWindowSize().Y };
                m_GraphicsDevice->EndImGuiRender(); 
            }            

            m_GraphicsDevice->SwapBuffers();      

            if (m_Specification.ImGuiActive)
            {
                //Update and render additional platform windows
                if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                    m_GraphicsDevice->SetContext();
                }
            }
        }

        m_Window->m_Input->CachePreviousInput();        
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