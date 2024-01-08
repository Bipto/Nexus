#include "Application.hpp"

// graphics headers
#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"
#include "Platform/D3D11/GraphicsDeviceD3D11.hpp"

#if defined(NX_PLATFORM_D3D12)
#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"
#endif

#if defined(NX_PLATFORM_VULKAN)
#include "Platform/Vulkan/GraphicsDeviceVk.hpp"
#endif

// audio headers
#if defined(NX_PLATFORM_OPENAL)
#include "Platform/OpenAL/AudioDeviceOpenAL.hpp"
#endif

#include "Nexus/Logging/Log.hpp"

#include "Nexus/Input/Input.hpp"

namespace Nexus
{
    static KeyCode SDLToNexusKeycode(SDL_Keycode keycode)
    {
        switch (keycode)
        {
        case SDLK_ESCAPE:
            return KeyCode::Escape;

        case SDLK_F1:
            return KeyCode::F1;
        case SDLK_F2:
            return KeyCode::F2;
        case SDLK_F3:
            return KeyCode::F3;
        case SDLK_F4:
            return KeyCode::F4;
        case SDLK_F5:
            return KeyCode::F5;
        case SDLK_F6:
            return KeyCode::F6;
        case SDLK_F7:
            return KeyCode::F7;
        case SDLK_F8:
            return KeyCode::F8;
        case SDLK_F9:
            return KeyCode::F9;
        case SDLK_F10:
            return KeyCode::F10;
        case SDLK_F11:
            return KeyCode::F11;
        case SDLK_F12:
            return KeyCode::F12;

        case SDLK_PRINTSCREEN:
            return KeyCode::PrintScreen;
        case SDLK_SCROLLLOCK:
            return KeyCode::ScrollLock;
        case SDLK_PAUSE:
            return KeyCode::PauseBreak;

        case SDLK_BACKQUOTE:
            return KeyCode::Tilde;
        case SDLK_1:
            return KeyCode::One;
        case SDLK_2:
            return KeyCode::Two;
        case SDLK_3:
            return KeyCode::Three;
        case SDLK_4:
            return KeyCode::Four;
        case SDLK_5:
            return KeyCode::Five;
        case SDLK_6:
            return KeyCode::Six;
        case SDLK_7:
            return KeyCode::Seven;
        case SDLK_8:
            return KeyCode::Eight;
        case SDLK_9:
            return KeyCode::Nine;
        case SDLK_0:
            return KeyCode::Zero;
        case SDLK_UNDERSCORE:
            return KeyCode::Underscore;
        case SDLK_EQUALS:
            return KeyCode::Equals;
        case SDLK_BACKSPACE:
            return KeyCode::Back;

        case SDLK_TAB:
            return KeyCode::Tab;
        case SDLK_q:
            return KeyCode::Q;
        case SDLK_w:
            return KeyCode::W;
        case SDLK_e:
            return KeyCode::E;
        case SDLK_r:
            return KeyCode::R;
        case SDLK_t:
            return KeyCode::T;
        case SDLK_y:
            return KeyCode::Y;
        case SDLK_u:
            return KeyCode::U;
        case SDLK_i:
            return KeyCode::I;
        case SDLK_o:
            return KeyCode::O;
        case SDLK_p:
            return KeyCode::P;
        case SDLK_LEFTBRACKET:
            return KeyCode::LeftBracket;
        case SDLK_RIGHTBRACKET:
            return KeyCode::RightBracket;
        case SDLK_RETURN:
            return KeyCode::Enter;

        case SDLK_CAPSLOCK:
            return KeyCode::CapsLock;
        case SDLK_a:
            return KeyCode::A;
        case SDLK_s:
            return KeyCode::S;
        case SDLK_d:
            return KeyCode::D;
        case SDLK_f:
            return KeyCode::F;
        case SDLK_g:
            return KeyCode::G;
        case SDLK_h:
            return KeyCode::H;
        case SDLK_j:
            return KeyCode::J;
        case SDLK_k:
            return KeyCode::K;
        case SDLK_l:
            return KeyCode::L;
        case SDLK_SEMICOLON:
            return KeyCode::SemiColon;
        case SDLK_QUOTE:
            return KeyCode::Apostrophe;
        case SDLK_HASH:
            return KeyCode::Hash;

        case SDLK_LSHIFT:
            return KeyCode::LeftShift;
        case SDLK_BACKSLASH:
            return KeyCode::Backslash;
        case SDLK_z:
            return KeyCode::Z;
        case SDLK_x:
            return KeyCode::X;
        case SDLK_c:
            return KeyCode::C;
        case SDLK_v:
            return KeyCode::V;
        case SDLK_b:
            return KeyCode::B;
        case SDLK_n:
            return KeyCode::N;
        case SDLK_m:
            return KeyCode::M;
        case SDLK_COMMA:
            return KeyCode::Comma;
        case SDLK_PERIOD:
            return KeyCode::Period;
        case SDLK_SLASH:
            return KeyCode::Slash;
        case SDLK_RSHIFT:
            return KeyCode::RightShift;

        case SDLK_LCTRL:
            return KeyCode::LeftControl;
        case SDLK_LGUI:
            return KeyCode::LeftWin;
        case SDLK_LALT:
            return KeyCode::LeftAlt;
        case SDLK_SPACE:
            return KeyCode::Space;
        case SDLK_RALT:
            return KeyCode::RightWin;
        case SDLK_RGUI:
            return KeyCode::PrintScreen;
        case SDLK_RCTRL:
            return KeyCode::RightControl;

        case SDLK_INSERT:
            return KeyCode::Insert;
        case SDLK_HOME:
            return KeyCode::Home;
        case SDLK_PAGEUP:
            return KeyCode::PageUp;
        case SDLK_DELETE:
            return KeyCode::Delete;
        case SDLK_END:
            return KeyCode::End;
        case SDLK_PAGEDOWN:
            return KeyCode::PageDown;

        case SDLK_LEFT:
            return KeyCode::KeyLeft;
        case SDLK_UP:
            return KeyCode::KeyUp;
        case SDLK_DOWN:
            return KeyCode::KeyDown;
        case SDLK_RIGHT:
            return KeyCode::KeyRight;

        case SDLK_NUMLOCKCLEAR:
            return KeyCode::NumLock;
        case SDLK_KP_DIVIDE:
            return KeyCode::KeyDivide;
        case SDLK_KP_MULTIPLY:
            return KeyCode::NumMultiply;
        case SDLK_KP_MEMSUBTRACT:
            return KeyCode::NumSubstract;
        case SDLK_KP_7:
            return KeyCode::Num7;
        case SDLK_KP_8:
            return KeyCode::Num8;
        case SDLK_KP_9:
            return KeyCode::Num9;
        case SDLK_KP_PLUS:
            return KeyCode::NumPlus;
        case SDLK_KP_4:
            return KeyCode::Num4;
        case SDLK_KP_5:
            return KeyCode::Num5;
        case SDLK_KP_6:
            return KeyCode::Num6;
        case SDLK_KP_1:
            return KeyCode::Num1;
        case SDLK_KP_2:
            return KeyCode::Num2;
        case SDLK_KP_3:
            return KeyCode::Num3;
        case SDLK_RETURN2:
            return KeyCode::NumEnter;
        case SDLK_KP_0:
            return KeyCode::Num0;
        case SDLK_KP_PERIOD:
            return KeyCode::NumDelete;

        default:
            return KeyCode::Unknown;
        }
    }

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

        WindowSpecification props;
        m_Window = CreateApplicationWindow(props, spec.SwapchainSpecification);

        Graphics::GraphicsDeviceCreateInfo graphicsDeviceCreateInfo;
        graphicsDeviceCreateInfo.API = spec.GraphicsAPI;

        m_GraphicsDevice = Nexus::CreateGraphicsDevice(graphicsDeviceCreateInfo, m_Window, spec.SwapchainSpecification);
        m_GraphicsDevice->SetContext();

        auto swapchain = m_Window->GetSwapchain();
        swapchain->SetVSyncState(spec.SwapchainSpecification.VSyncState);

        m_AudioDevice = Nexus::CreateAudioDevice(spec.AudioAPI);

        if (spec.ImGuiActive)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            ImGuiIO &io = ImGui::GetIO();
            (void)io;
            // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            m_ImGuiRenderer = new Nexus::Graphics::ImGuiRenderer(this);
            m_ImGuiRenderer->Initialise();
        }
    }

    Application::~Application()
    {
        if (m_Specification.ImGuiActive)
        {
            m_ImGuiRenderer->Shutdown();
        }

        delete m_AudioDevice;
        delete m_GraphicsDevice;
    }

    void Application::MainLoop()
    {
        CloseWindows();

        if (m_Window->m_SwapchainRequiresResize)
        {
            OnResize(this->GetWindowSize());
        }

        this->PollEvents(m_Specification.ImGuiActive);

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

        CheckForClosingWindows();
    }

    Nexus::Window *Application::GetPrimaryWindow()
    {
        return m_Window;
    }

    Point<uint32_t> Application::GetWindowSize()
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
        m_Window->Close();
    }

    bool Application::ShouldClose()
    {
        return m_Window->IsClosing();
    }

    Window *Application::CreateApplicationWindow(const WindowSpecification &windowProps, const Graphics::SwapchainSpecification &swapchainSpec)
    {
        Window *window = new Nexus::Window(windowProps, m_Specification.GraphicsAPI, swapchainSpec);
        m_Windows.push_back(window);
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

    void Application::PollEvents(bool imguiActive)
    {
        for (auto window : m_Windows)
        {
            window->m_Input->CacheInput();
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (imguiActive)
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
            }

            auto window = GetWindowFromHandle(event.window.windowID);

            switch (event.type)
            {
            case SDL_KEYDOWN:
            {
                auto nexusKeyCode = SDLToNexusKeycode(event.key.keysym.sym);
                window->m_Input->m_Keyboard.m_CurrentKeys[nexusKeyCode] = true;
                break;
            }
            case SDL_KEYUP:
            {
                auto nexusKeyCode = SDLToNexusKeycode(event.key.keysym.sym);
                window->m_Input->m_Keyboard.m_CurrentKeys[nexusKeyCode] = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                {
                    window->m_Input->m_Mouse.m_CurrentState.LeftButton = MouseButtonState::Pressed;
                    break;
                }
                case SDL_BUTTON_RIGHT:
                {
                    window->m_Input->m_Mouse.m_CurrentState.RightButton = MouseButtonState::Pressed;
                    break;
                }
                case SDL_BUTTON_MIDDLE:
                {
                    window->m_Input->m_Mouse.m_CurrentState.MiddleButton = MouseButtonState::Pressed;
                    break;
                }
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                {
                    window->m_Input->m_Mouse.m_CurrentState.LeftButton = MouseButtonState::Released;
                    break;
                }
                case SDL_BUTTON_RIGHT:
                {
                    window->m_Input->m_Mouse.m_CurrentState.RightButton = MouseButtonState::Released;
                    break;
                }
                case SDL_BUTTON_MIDDLE:
                {
                    window->m_Input->m_Mouse.m_CurrentState.MiddleButton = MouseButtonState::Released;
                    break;
                }
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;
                window->m_Input->m_Mouse.m_CurrentState.MousePosition = {
                    mouseX,
                    mouseY};

                break;
            }
            case SDL_MOUSEWHEEL:
            {
                auto &scroll = window->m_Input->m_Mouse.m_CurrentState.MouseWheel;
                scroll.X += event.wheel.x;
                scroll.Y += event.wheel.y;
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    window->m_SwapchainRequiresResize = true;
                    break;
                }

                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    window->Close();
                    break;
                }
                break;
            }
            case SDL_CONTROLLERDEVICEADDED:
            {
                if (SDL_IsGameController(event.cdevice.which))
                {
                    Nexus::Input::AddController(event.cdevice.which);
                    break;
                }
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED:
            {
                Nexus::Input::RemoveController(event.cdevice.which);
                break;
            }
            case SDL_TEXTINPUT:
            {
                window->m_Input->TextInput.Invoke(event.text.text[0]);
                break;
            }
            }
        }
    }

    Window *Application::GetWindowFromHandle(uint32_t handle)
    {
        for (int i = 0; i < m_Windows.size(); i++)
        {
            if (m_Windows[i]->GetID() == handle)
            {
                return m_Windows[i];
            }
        }
        return nullptr;
    }

    void Application::CheckForClosingWindows()
    {
        uint32_t indexToRemove = 0;
        for (uint32_t i = 0; i < m_Windows.size(); i++)
        {
            if (m_Windows[i]->IsClosing())
            {
                auto window = m_Windows[i];

                auto pair = std::make_pair(window, 0);
                m_WindowsToClose.push_back(pair);
            }
        }
    }

    void Application::CloseWindows()
    {
        // this is required because the swapchain's framebuffer may still be in use on the GPU
        // we need to wait until we are certain that the swapchain is no longer in use before we delete it
        for (uint32_t i = 0; i < m_WindowsToClose.size(); i++)
        {
            m_WindowsToClose[i].second++;
        }

        for (uint32_t closingWindowIndex = 0; closingWindowIndex < m_WindowsToClose.size(); closingWindowIndex++)
        {
            auto pair = m_WindowsToClose[closingWindowIndex];

            auto window = pair.first;
            auto count = pair.second;

            if (count > 10)
            {
                for (uint32_t windowIndex = 0; windowIndex < m_Windows.size(); windowIndex++)
                {
                    if (window == m_Windows[windowIndex])
                    {
                        delete window;
                        window = nullptr;

                        m_WindowsToClose.erase(m_WindowsToClose.begin() + closingWindowIndex);
                        m_Windows.erase(m_Windows.begin() + windowIndex);
                    }
                }
            }
        }
    }

    Graphics::GraphicsDevice *CreateGraphicsDevice(const Graphics::GraphicsDeviceCreateInfo &createInfo, Window *window, const Graphics::SwapchainSpecification &swapchainSpec)
    {
        switch (createInfo.API)
        {
#if defined(NX_PLATFORM_D3D11)
        case Graphics::GraphicsAPI::D3D11:
            return new Graphics::GraphicsDeviceD3D11(createInfo, window, swapchainSpec);
            break;
#endif

#if defined(NX_PLATFORM_D3D12)
        case Graphics::GraphicsAPI::D3D12:
            return new Graphics::GraphicsDeviceD3D12(createInfo, window, swapchainSpec);
            break;
#endif

        case Graphics::GraphicsAPI::OpenGL:
            return new Graphics::GraphicsDeviceOpenGL(createInfo, window, swapchainSpec);
            break;

#if defined(NX_PLATFORM_VULKAN)
        case Graphics::GraphicsAPI::Vulkan:
            return new Graphics::GraphicsDeviceVk(createInfo, window, swapchainSpec);
            break;
#endif
        default:
            return nullptr;
            break;
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