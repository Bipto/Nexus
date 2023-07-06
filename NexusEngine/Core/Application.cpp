#include "Application.h"

#include "Platform/OpenGL/GraphicsDeviceOpenGL.h"
#include "Platform/DX11/GraphicsDeviceDX11.h"

#include "Platform/OpenAL/AudioDeviceOpenAL.h"

#include "SDL_opengl.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    void Clock::Tick()
    {
        std::chrono::high_resolution_clock::time_point tickTime = std::chrono::high_resolution_clock::now();
        m_DeltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(tickTime - m_StartTime).count();
        m_StartTime = tickTime;
    }

    Nexus::Time Clock::GetTime()
    {
        return Nexus::Time(m_DeltaTime);
    }

    Application::Application(const ApplicationSpecification &spec)
    {
        m_Specification = spec;

        WindowProperties props;
        this->m_Window = new Nexus::Window(props);

        Graphics::GraphicsDeviceCreateInfo graphicsDeviceCreateInfo;
        graphicsDeviceCreateInfo.GraphicsWindow = m_Window;
        graphicsDeviceCreateInfo.API = spec.GraphicsAPI;
        graphicsDeviceCreateInfo.VSyncStateSettings = spec.VSyncState;

        Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = m_Window->GetWindowSize().X;
        vp.Height = m_Window->GetWindowSize().Y;
        graphicsDeviceCreateInfo.GraphicsViewport = vp;

        m_GraphicsDevice = Nexus::CreateGraphicsDevice(graphicsDeviceCreateInfo);
        m_GraphicsDevice->SetContext();
        m_GraphicsDevice->Resize(GetWindowSize());
        m_GraphicsDevice->SetVSyncState(spec.VSyncState);

        m_AudioDevice = Nexus::CreateAudioDevice(spec.AudioAPI);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        if (m_GraphicsDevice->GetGraphicsAPI() == Graphics::GraphicsAPI::OpenGL)
        {
            SDL_Window *window = this->m_Window->GetSDLWindowHandle();
            SDL_GLContext context = (SDL_GLContext)this->m_GraphicsDevice->GetContext();

            ImGui_ImplSDL2_InitForOpenGL(window, context);
        }

        else if (m_GraphicsDevice->GetGraphicsAPI() == Graphics::GraphicsAPI::DirectX11)
        {
            ImGui_ImplSDL2_InitForD3D(this->m_Window->GetSDLWindowHandle());
        }

        m_GraphicsDevice->InitialiseImGui();
    }

    Application::~Application()
    {
        delete this->m_Window;
    }

    void Application::MainLoop()
    {
        if (m_Window->m_SwapchainRequiresResize)
        {
            OnResize(this->GetWindowSize());
            m_Window->m_SwapchainRequiresResize = false;
            m_PreviousWindowSize = this->GetWindowSize();
        }

        this->m_Window->PollEvents(m_Specification.ImGuiActive);

        // Allow user to block closing events, for example to display save prompt
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

        // if vsync is disabled, check if we should render yet
        if (m_GraphicsDevice->GetVsyncState() == Nexus::Graphics::VSyncState::Disabled)
            if (m_RenderTimer < timeBetweenRenders)
                return;

        // run render functions
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
                Nexus::Graphics::Viewport vp;
                vp.X = 0;
                vp.Y = 0;
                vp.Width = m_Window->GetWindowSize().X;
                vp.Height = m_Window->GetWindowSize().Y;
                m_GraphicsDevice->SetViewport(vp);

                ImGui::Render();
                ImGui::GetMainViewport()->Size = {(float)this->GetWindowSize().X, (float)this->GetWindowSize().Y};
                m_GraphicsDevice->EndImGuiRender();
            }

            m_GraphicsDevice->SwapBuffers();

            if (m_Specification.ImGuiActive)
            {
                // Update and render additional platform windows
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

    Nexus::Window *Application::GetWindow()
    {
        return m_Window;
    }

    Point<int> Application::GetWindowSize()
    {
        return this->m_Window->GetWindowSize();
    }

    Point<int> Application::GetWindowPosition()
    {
        return this->m_Window->GetWindowPosition();
    }

    bool Application::IsWindowFocussed()
    {
        return m_Window->IsFocussed();
    }

    WindowState Application::GetCurrentWindowState()
    {
        return m_Window->GetCurrentWindowState();
    }

    void Application::SetIsMouseVisible(bool visible)
    {
        m_Window->SetIsMouseVisible(visible);
    }

    void Application::SetCursor(Cursor cursor)
    {
        m_Window->SetCursor(cursor);
    }

    void Application::Close()
    {
        this->m_Window->Close();
    }

    bool Application::ShouldClose()
    {
        return this->m_Window->IsClosing();
    }

    Window *Application::CreateApplicationWindow(const WindowProperties &props)
    {
        Window *window = new Nexus::Window(props);
        return window;
    }

    InputState *Application::GetCoreInputState()
    {
        return m_Window->GetInput();
    }

    Ref<Graphics::GraphicsDevice> Application::GetGraphicsDevice()
    {
        return m_GraphicsDevice;
    }

    Ref<Graphics::GraphicsDevice> CreateGraphicsDevice(const Graphics::GraphicsDeviceCreateInfo &createInfo)
    {
        switch (createInfo.API)
        {
        case Graphics::GraphicsAPI::DirectX11:
            return CreateRef<Graphics::GraphicsDeviceDX11>(createInfo);
        default:
            return CreateRef<Graphics::GraphicsDeviceOpenGL>(createInfo);
        }
    }

    Ref<Audio::AudioDevice> CreateAudioDevice(Audio::AudioAPI api)
    {
        switch (api)
        {
        default:
            return CreateRef<Audio::AudioDeviceOpenAL>();
        }
    }
}