#include "Application.hpp"

// graphics headers
#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"
#include "Platform/DX11/GraphicsDeviceDX11.hpp"

#if defined(NX_PLATFORM_VULKAN)
#include "Platform/Vulkan/GraphicsDeviceVk.hpp"
#endif

// audio headers
#if defined(NX_PLATFORM_OPENAL)
#include "Platform/OpenAL/AudioDeviceOpenAL.hpp"
#endif

#include "Nexus/Logging/Log.hpp"

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
        props.GraphicsAPI = spec.GraphicsAPI;
        m_Window = new Nexus::Window(props, spec.GraphicsAPI);

        Graphics::GraphicsDeviceCreateInfo graphicsDeviceCreateInfo;
        graphicsDeviceCreateInfo.API = spec.GraphicsAPI;
        graphicsDeviceCreateInfo.VSyncStateSettings = spec.VSyncState;

        m_GraphicsDevice = Nexus::CreateGraphicsDevice(graphicsDeviceCreateInfo, m_Window);
        m_GraphicsDevice->SetContext();

        auto swapchain = m_Window->GetSwapchain();
        swapchain->SetVSyncState(spec.VSyncState);

        m_AudioDevice = Nexus::CreateAudioDevice(spec.AudioAPI);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        m_ImGuiRenderer = new Nexus::Graphics::ImGuiRenderer(this);
        m_ImGuiRenderer->Initialise();
    }

    Application::~Application()
    {
        m_ImGuiRenderer->Shutdown();
        delete m_Window;
        delete m_AudioDevice;
        delete m_GraphicsDevice;
    }

    void Application::MainLoop()
    {
        if (m_Window->m_SwapchainRequiresResize)
        {
            OnResize(this->GetWindowSize());
        }

        this->m_Window->PollEvents(m_Specification.ImGuiActive);

        // Allow user to block closing events, for example to display save prompt
        if (m_Window->m_Closing)
            m_Window->m_Closing = this->OnClose();

        m_Clock.Tick();
        auto time = m_Clock.GetTime();

        this->Update(time);

        // run render functions
        {
            if (m_Specification.ImGuiActive)
            {
                m_ImGuiRenderer->BeginFrame();
            }

            this->Render(time);

            if (m_Specification.ImGuiActive)
            {
                ImGui::Render();
                ImGui::GetMainViewport()->Size = {(float)this->GetWindowSize().X, (float)this->GetWindowSize().Y};
                m_ImGuiRenderer->EndFrame();
            }

            auto swapchain = m_Window->GetSwapchain();
            swapchain->SwapBuffers();

            if (m_Specification.ImGuiActive)
            {
                // Update and render additional platform windows
                m_ImGuiRenderer->UpdatePlatformWindows();
            }
        }

        m_Window->m_Input->CachePreviousInput();
    }

    Nexus::Window *Application::GetPrimaryWindow()
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
        Window *window = new Nexus::Window(props, m_Specification.GraphicsAPI);
        return window;
    }

    InputState *Application::GetCoreInputState()
    {
        return m_Window->GetInput();
    }

    Graphics::GraphicsDevice *Application::GetGraphicsDevice()
    {
        return m_GraphicsDevice;
    }

    Audio::AudioDevice *Application::GetAudioDevice()
    {
        return m_AudioDevice;
    }

    Graphics::GraphicsDevice *CreateGraphicsDevice(const Graphics::GraphicsDeviceCreateInfo &createInfo, Window *window)
    {
        switch (createInfo.API)
        {
        case Graphics::GraphicsAPI::DirectX11:
#if defined(NX_PLATFORM_DX11)
            return new Graphics::GraphicsDeviceDX11(createInfo, window);
#else
            return nullptr;
#endif

        case Graphics::GraphicsAPI::OpenGL:
            return new Graphics::GraphicsDeviceOpenGL(createInfo, window);

        case Graphics::GraphicsAPI::Vulkan:
#if defined(NX_PLATFORM_VULKAN)
            return new Graphics::GraphicsDeviceVk(createInfo, window);
#else
            return nullptr;
#endif
        }
    }

    Audio::AudioDevice *CreateAudioDevice(Audio::AudioAPI api)
    {
        switch (api)
        {
#if defined(NX_PLATFORM_OPENAL)
        case Audio::AudioAPI::OpenAL:
            return new Audio::AudioDeviceOpenAL();
#endif

        default:
            return nullptr;
        }
    }
}