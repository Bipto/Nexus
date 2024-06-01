#include "Window.hpp"

#include "glad/glad.h"

#include "Nexus/Graphics/GraphicsDevice.hpp"

#if defined(NX_PLATFORM_OPENGL)
#include "Platform/OpenGL/SwapchainOpenGL.hpp"
#endif

#if defined(NX_PLATFORM_VULKAN)
#include "Platform/Vulkan/SwapchainVk.hpp"
#endif

#if defined(NX_PLATFORM_D3D12)
#include "Platform/D3D12/SwapchainD3D12.hpp"
#endif

namespace Nexus
{

    Window::Window(const WindowSpecification &windowProps, Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec)
    {
        // NOTE: Resizable flag MUST be set in order for Emscripten resizing to work correctly
        uint32_t flags = GetFlags(api, windowProps, swapchainSpec);

        m_Window = SDL_CreateWindow(windowProps.Title.c_str(), windowProps.Width, windowProps.Height, flags);

        if (m_Window == nullptr)
        {
            std::string errorCode = {SDL_GetError()};
            NX_ERROR(errorCode);
        }

        m_Input = new InputState();
        m_WindowID = SDL_GetWindowID(m_Window);

        SDL_StartTextInput();
    }

    Window::~Window()
    {
        SDL_StopTextInput();
        delete m_Swapchain;
        SDL_DestroyWindow(this->m_Window);
    }

    void Window::SetResizable(bool isResizable)
    {
        SDL_SetWindowResizable(this->m_Window, (SDL_bool)isResizable);
    }

    void Window::SetTitle(const std::string &title)
    {
        SDL_SetWindowTitle(this->m_Window, title.c_str());
    }

    void Window::SetSize(Point<uint32_t> size)
    {
#if !defined(__EMSCRIPTEN__)
        SDL_SetWindowSize(m_Window, size.X, size.Y);
#else
        SDL_SetWindowSize(m_Window, size.X * GetDisplayScale(), size.Y * GetDisplayScale());
#endif
    }

    void Window::Close()
    {
        m_Closing = true;
    }

    bool Window::IsClosing()
    {
        return m_Closing;
    }

    SDL_Window *Window::GetSDLWindowHandle()
    {
        return m_Window;
    }

    Point<uint32_t> Window::GetWindowSize()
    {
        int x, y;
        SDL_GetWindowSize(m_Window, &x, &y);

        Point<uint32_t> size{};
        size.X = x;
        size.Y = y;

#if defined(__EMSCRIPTEN__)
        size.X *= GetDisplayScale();
        size.Y *= GetDisplayScale();
#endif

        return size;
    }

    Point<int> Window::GetWindowPosition()
    {
        Point<int> position{};
        SDL_GetWindowPosition(m_Window, &position.X, &position.Y);
        return position;
    }

    WindowState Window::GetCurrentWindowState()
    {
        Uint32 flags = SDL_GetWindowFlags(m_Window);

        if (flags & SDL_WINDOW_MAXIMIZED)
        {
            m_CurrentWindowState = WindowState::Maximized;
        }
        else if (flags & SDL_WINDOW_MINIMIZED)
        {
            m_CurrentWindowState = WindowState::Minimized;
        }
        else
        {
            m_CurrentWindowState = WindowState::Normal;
        }

        return m_CurrentWindowState;
    }

    void Window::SetIsMouseVisible(bool visible)
    {
        switch (visible)
        {
        case true:
            SDL_ShowCursor();
            break;
        case false:
            SDL_HideCursor();
            break;
        }
    }

    void Window::SetCursor(Cursor cursor)
    {
        SDL_Cursor *sdlCursor;

        switch (cursor)
        {
        case Cursor::Arrow:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
            break;
        case Cursor::IBeam:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
            break;
        case Cursor::Wait:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
            break;
        case Cursor::Crosshair:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
            break;
        case Cursor::WaitArrow:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
            break;
        case Cursor::ArrowNWSE:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
            break;
        case Cursor::ArrowNESW:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
            break;
        case Cursor::ArrowWE:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
            break;
        case Cursor::ArrowNS:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
            break;
        case Cursor::ArrowAllDir:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
            break;
        case Cursor::No:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
            break;
        case Cursor::Hand:
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
            break;
        }

        SDL_SetCursor(sdlCursor);
    }

    InputState *Window::GetInput()
    {
        return m_Input;
    }

    bool Window::IsFocussed()
    {
        return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_INPUT_FOCUS;
    }

    void Window::Maximize()
    {
        SDL_MaximizeWindow(m_Window);
    }

    void Window::Minimize()
    {
        SDL_MinimizeWindow(m_Window);
    }

    void Window::Restore()
    {
        SDL_RestoreWindow(m_Window);
    }

    void Window::ToggleFullscreen()
    {
        if (IsFullscreen())
        {
            UnsetFullscreen();
        }
        else
        {
            SetFullscreen();
        }
    }

    bool Window::IsFullscreen()
    {
        auto flags = SDL_GetWindowFlags(m_Window);

        if (flags & SDL_WINDOW_FULLSCREEN)
        {
            return true;
        }

        return false;
    }

    void Window::SetFullscreen()
    {
        SDL_SetWindowFullscreen(m_Window, SDL_TRUE);
    }

    void Window::UnsetFullscreen()
    {
        SDL_SetWindowFullscreen(m_Window, SDL_FALSE);
    }

    void Window::Show()
    {
        SDL_ShowWindow(m_Window);
    }

    void Window::Hide()
    {
        SDL_HideWindow(m_Window);
    }

    void Window::SetWindowPosition(int32_t x, int32_t y)
    {
        SDL_SetWindowPosition(m_Window, x, y);
    }

    void Window::Focus()
    {
        SDL_RaiseWindow(m_Window);
    }

    void Window::CreateSwapchain(Graphics::GraphicsDevice *device, const Graphics::SwapchainSpecification &swapchainSpec)
    {
        switch (device->GetGraphicsAPI())
        {
#if defined(NX_PLATFORM_OPENGL)
        case Graphics::GraphicsAPI::OpenGL:
        {
            m_Swapchain = new Graphics::SwapchainOpenGL(this, swapchainSpec);
            break;
        }
#endif
#if defined(NX_PLATFORM_VULKAN)
        case Graphics::GraphicsAPI::Vulkan:
        {
            m_Swapchain = new Graphics::SwapchainVk(this, device, swapchainSpec);
            break;
        }
#endif
#if defined(NX_PLATFORM_D3D12)
        case Graphics::GraphicsAPI::D3D12:
        {
            m_Swapchain = new Graphics::SwapchainD3D12(this, device, swapchainSpec);
            break;
        }
#endif

        default:
            NX_ERROR("Failed to create swapchain");
            throw std::runtime_error("Failed to create swapchain");
        }
    }

    Graphics::Swapchain *Window::GetSwapchain()
    {
        return m_Swapchain;
    }

    uint32_t Window::GetID()
    {
        return m_WindowID;
    }

    float Window::GetDisplayScale()
    {
        return SDL_GetWindowDisplayScale(m_Window);
    }

    void Window::SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect)
    {
        SDL_Rect r;
        r.x = rect.GetLeft();
        r.y = rect.GetTop();
        r.w = rect.GetWidth();
        r.h = rect.GetHeight();
        SDL_SetTextInputRect(&r);
    }

    void Window::StartTextInput()
    {
    }

    void Window::StopTextInput()
    {
    }

    uint32_t Window::GetFlags(Graphics::GraphicsAPI api, const WindowSpecification &windowSpec, const Graphics::SwapchainSpecification &swapchainSpec)
    {
        // required for emscripten to handle resizing correctly
        uint32_t flags = 0;
        flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

        if (windowSpec.Resizable)
        {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        if (windowSpec.Borderless)
        {
            flags |= SDL_WINDOW_BORDERLESS;
        }

        if (windowSpec.Utility)
        {
            flags |= SDL_WINDOW_UTILITY;
        }

        switch (api)
        {
            // setup for OpenGL
#if defined(NX_PLATFORM_OPENGL)
        case Graphics::GraphicsAPI::OpenGL:
        {
#if defined(__EMSCRIPTEN__) || defined(__ANDROID) || defined(ANDROID)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

#if defined(_DEBUG)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
#endif

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

            if (swapchainSpec.Samples != Graphics::SampleCount::SampleCount1)
            {
                uint32_t samples = Graphics::GetSampleCount(swapchainSpec.Samples);
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
            }
            else
            {
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
                SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);
            }

#if defined(NX_PLATFORM_SUPPORTS_MULTI_WINDOW)
            if (Graphics::SwapchainOpenGL::HasContextBeenCreated())
            {
                SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            }
            else
            {
                SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
            }
#endif

            flags |= SDL_WINDOW_OPENGL;
            return flags;
        }
#endif
        case Graphics::GraphicsAPI::Vulkan:
        {
            flags |= SDL_WINDOW_VULKAN;
            return flags;
        }
        default:
            return flags;
        }
    }
}