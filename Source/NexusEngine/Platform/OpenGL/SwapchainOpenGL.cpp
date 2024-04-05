#if defined(NX_PLATFORM_OPENGL)

#include "SwapchainOpenGL.hpp"

#include "GL.hpp"
#include "SDL.h"

#include "BufferOpenGL.hpp"

namespace Nexus::Graphics
{
    SwapchainOpenGL::SwapchainOpenGL(Window *window, const SwapchainSpecification &swapchainSpec)
        : Swapchain(swapchainSpec), m_Window(window), m_VsyncState(swapchainSpec.VSyncState)
    {
        m_SwapchainWidth = m_Window->GetWindowSize().X;
        m_SwapchainHeight = m_Window->GetWindowSize().Y;

#if defined(NX_PLATFORM_SUPPORTS_MULTI_WINDOW)

        if (!s_ContextWindow)
        {
            s_ContextWindow = window->GetSDLWindowHandle();
        }

        m_Context = SDL_GL_CreateContext(s_ContextWindow);

        if (!s_MainContext)
        {
            s_MainContext = m_Context;
        }

        if (m_Context == NULL)
        {
            std::string error = {SDL_GetError()};
            NX_ERROR(error);
        }
#else
        s_ContextWindow = window->GetSDLWindowHandle();

        m_Context = SDL_GL_CreateContext(s_ContextWindow);
        if (m_Context == NULL)
        {
            std::string error = {SDL_GetError()};
            NX_ERROR(error);
        }
#endif

        if (SDL_GL_MakeCurrent(window->GetSDLWindowHandle(), m_Context) != 0)
        {
            std::string error = SDL_GetError();
            throw std::runtime_error(error);
        }

        std::cout << "Context address: " << m_Context << std::endl;
    }

    SwapchainOpenGL::~SwapchainOpenGL()
    {
    }

    void SwapchainOpenGL::SwapBuffers()
    {
        BindAsRenderTarget();

        SDL_GL_SwapWindow(m_Window->GetSDLWindowHandle());
        ResizeIfNecessary();
    }

    VSyncState SwapchainOpenGL::GetVsyncState()
    {
        return m_VsyncState;
    }

    void SwapchainOpenGL::SetVSyncState(VSyncState vsyncState)
    {
        m_VsyncState = vsyncState;
        SDL_GL_SetSwapInterval((unsigned int)m_VsyncState);
    }

    void SwapchainOpenGL::ResizeIfNecessary()
    {
        int w, h;
        SDL_GetWindowSizeInPixels(m_Window->GetSDLWindowHandle(), &w, &h);

        glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer);
        glViewport(0, 0, w, h);

        m_SwapchainWidth = w;
        m_SwapchainHeight = h;
    }

    void SwapchainOpenGL::BindAsRenderTarget()
    {
        if (SDL_GL_MakeCurrent(m_Window->GetSDLWindowHandle(), m_Context) != 0)
        {
            std::string error = SDL_GetError();
            throw std::runtime_error(error);
        }

        ResizeIfNecessary();
        glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer);

        glViewport(0, 0, m_SwapchainWidth, m_SwapchainHeight);
        glScissor(0, 0, m_SwapchainWidth, m_SwapchainHeight);
    }

    void SwapchainOpenGL::BindAsDrawTarget()
    {
        if (SDL_GL_MakeCurrent(m_Window->GetSDLWindowHandle(), m_Context) != 0)
        {
            std::string error = SDL_GetError();
            throw std::runtime_error(error);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Backbuffer);
    }

    void SwapchainOpenGL::BindDefaultSwapchain()
    {
        if (SDL_GL_MakeCurrent(s_ContextWindow, s_MainContext) != 0)
        {
            std::string error = SDL_GetError();
            throw std::runtime_error(error);
        }
    }

    // static member initialisation
    SDL_Window *SwapchainOpenGL::s_ContextWindow = nullptr;
    SDL_GLContext SwapchainOpenGL::s_MainContext = nullptr;

    void SwapchainOpenGL::Prepare()
    {
    }

    bool SwapchainOpenGL::HasContextBeenCreated()
    {
        return s_MainContext;
    }
}

#endif