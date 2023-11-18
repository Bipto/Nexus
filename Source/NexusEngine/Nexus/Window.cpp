#include "Window.hpp"

#include "glad/glad.h"

#include "SDL_syswm.h"

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "Platform/OpenGL/SwapchainOpenGL.hpp"

#if defined(NX_PLATFORM_D3D11)
#include "Platform/D3D11/SwapchainD3D11.hpp"
#endif

#if defined(NX_PLATFORM_VULKAN)
#include "Platform/Vulkan/SwapchainVk.hpp"
#endif

#if defined(NX_PLATFORM_D3D12)
#include "Platform/D3D12/SwapchainD3D12.hpp"
#endif

namespace Nexus
{

    Window::Window(const WindowProperties &windowProps, Graphics::GraphicsAPI api)
    {
        uint32_t flags = GetFlags(api);

        // NOTE: Resizable flag MUST be set in order for Emscripten resizing to work correctly
        m_Window = SDL_CreateWindow(windowProps.Title.c_str(),
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    windowProps.Width,
                                    windowProps.Height,
                                    flags);

        m_Input = new InputState();
        m_WindowID = SDL_GetWindowID(m_Window);
    }

    Window::~Window()
    {
        if (m_Swapchain)
        {
            delete m_Swapchain;
        }

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

    void Window::SetSize(Point<int> size)
    {
        SDL_SetWindowSize(m_Window, size.X, size.Y);
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

    Point<int> Window::GetWindowSize()
    {
        Point<int> size{};
#ifndef __EMSCRIPTEN__
        SDL_GetWindowSize(m_Window, &size.X, &size.Y);
#else
        emscripten_get_canvas_element_size("canvas", &size.X, &size.Y);
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
        if (flags & SDL_WINDOW_INPUT_FOCUS)
        {
            m_IsFocussed = true;
        }
        else
        {
            m_IsFocussed = false;
        }

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
            SDL_ShowCursor(SDL_ENABLE);
            break;
        case false:
            SDL_ShowCursor(SDL_DISABLE);
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
        return m_IsFocussed;
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

    void Window::CreateSwapchain(Graphics::GraphicsDevice *device, Graphics::VSyncState vSyncState)
    {
        switch (device->GetGraphicsAPI())
        {
        case Graphics::GraphicsAPI::OpenGL:
        {
            m_Swapchain = new Graphics::SwapchainOpenGL(this, vSyncState);
            break;
        }
#if defined(NX_PLATFORM_D3D11)
        case Graphics::GraphicsAPI::D3D11:
        {
            m_Swapchain = new Graphics::SwapchainD3D11(this, device, vSyncState);
            break;
        }
#endif
#if defined(NX_PLATFORM_VULKAN)
        case Graphics::GraphicsAPI::Vulkan:
        {
            m_Swapchain = new Graphics::SwapchainVk(this, device, vSyncState);
            break;
        }
#endif
#if defined(NX_PLATFORM_D3D12)
        case Graphics::GraphicsAPI::D3D12:
        {
            m_Swapchain = new Graphics::SwapchainD3D12(this, device, vSyncState);
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

    uint32_t Window::GetFlags(Graphics::GraphicsAPI api)
    {
        // required for emscripten to handle resizing correctly
        uint32_t flags = SDL_WINDOW_RESIZABLE;

        switch (api)
        {
            // setup for OpenGL
        case Graphics::GraphicsAPI::OpenGL:
        {
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

#ifndef __EMSCRIPTEN__
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

            if (Graphics::SwapchainOpenGL::HasContextBeenCreated())
            {
                SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            }
            else
            {
                SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
            }

            flags |= SDL_WINDOW_OPENGL;
            return flags;
        }
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