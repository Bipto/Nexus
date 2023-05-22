#include "Window.h"

#include "glad/glad.h"

using namespace Nexus;
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

namespace Nexus
{
    Window::Window(const WindowProperties &windowProps)
    {
        // NOTE: Resizable flag MUST be set in order for Emscripten resizing to work correctly
        this->m_Window = SDL_CreateWindow(windowProps.Title.c_str(),
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          windowProps.Width,
                                          windowProps.Height,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        m_Input = new InputState();
    }

    Window::~Window()
    {
        SDL_DestroyWindow(this->m_Window);
    }

    void Window::PollEvents(bool imguiActive)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (imguiActive)
                ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_QUIT:
            {
                this->Close();
                break;
            }
            case SDL_KEYDOWN:
            {
                auto nexusKeyCode = SDLToNexusKeycode(event.key.keysym.sym);
                m_Input->m_Keyboard.m_CurrentKeys[nexusKeyCode] = true;
                break;
            }
            case SDL_KEYUP:
            {
                auto nexusKeyCode = SDLToNexusKeycode(event.key.keysym.sym);
                m_Input->m_Keyboard.m_CurrentKeys[nexusKeyCode] = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    m_Input->m_Mouse.m_CurrentState.LeftButton = MouseButtonState::Pressed;
                    break;
                case SDL_BUTTON_RIGHT:
                    m_Input->m_Mouse.m_CurrentState.RightButton = MouseButtonState::Pressed;
                    break;
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    m_Input->m_Mouse.m_CurrentState.LeftButton = MouseButtonState::Released;
                    break;
                case SDL_BUTTON_RIGHT:
                    m_Input->m_Mouse.m_CurrentState.RightButton = MouseButtonState::Released;

                    break;
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;

                m_Input->m_Mouse.m_CurrentState.MousePosition = {
                    mouseX,
                    mouseY};
                break;
            }
            case SDL_MOUSEWHEEL:
            {
                auto scroll = m_Input->m_Mouse.m_CurrentState.MouseWheel;
                scroll.X += event.wheel.x;
                scroll.Y += event.wheel.y;
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    m_RequiresResize = true;

                break;
            }
            case SDL_CONTROLLERDEVICEADDED:
            {
                if (SDL_IsGameController(event.cdevice.which))
                {
                    m_Input->ConnectController(event.cdevice.which);
                }
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED:
            {
                m_Input->RemoveController(event.cdevice.which);
                break;
            }
            }
        }
    }

    void Window::SetResizable(bool isResizable)
    {
        SDL_SetWindowResizable(this->m_Window, (SDL_bool)isResizable);
    }

    void Window::SetTitle(const std::string &title)
    {
        SDL_SetWindowTitle(this->m_Window, title.c_str());
    }

    void Window::SetSize(Point<int> size)
    {
        SDL_SetWindowSize(this->m_Window, size.X, size.Y);
    }

    Point<int> Window::GetWindowSize()
    {
        Point<int> size{};
#ifndef __EMSCRIPTEN__
        SDL_GetWindowSize(this->m_Window, &size.X, &size.Y);
#else
        emscripten_get_canvas_element_size("canvas", &size.X, &size.Y);
#endif
        return size;
    }

    Point<int> Window::GetWindowPosition()
    {
        Point<int> position{};
        SDL_GetWindowPosition(this->m_Window, &position.X, &position.Y);
        return position;
    }
}