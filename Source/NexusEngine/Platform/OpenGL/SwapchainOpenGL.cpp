#if defined(NX_PLATFORM_OPENGL)

#include "SwapchainOpenGL.hpp"

#include "GL.hpp"
#include "SDL.h"

namespace Nexus::Graphics
{
    SwapchainOpenGL::SwapchainOpenGL(Window *window, const SwapchainSpecification &swapchainSpec)
        : Swapchain(swapchainSpec), m_Window(window), m_VsyncState(swapchainSpec.VSyncState)
    {
        m_SwapchainWidth = m_Window->GetWindowSize().X;
        m_SwapchainHeight = m_Window->GetWindowSize().Y;

        if (!s_ContextCreated)
        {
            s_ContextWindow = window->GetSDLWindowHandle();
            s_ContextCreated = true;
        }

        /* #if defined(EMSCRIPTEN)
                EmscriptenWebGLContextAttributes attrs;
                attrs.antialias = true;
                attrs.majorVersion = 3;
                attrs.minorVersion = 2;
                attrs.alpha = true;
                EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context = emscripten_webgl_create_context("#canvas", &attrs);
                emscripten_webgl_make_context_current(webgl_context);
        #endif */

        m_Context = SDL_GL_CreateContext(s_ContextWindow);

        SDL_GL_MakeCurrent(window->GetSDLWindowHandle(), m_Context);
    }

    SwapchainOpenGL::~SwapchainOpenGL()
    {
        SDL_GL_DeleteContext(m_Context);
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
        auto windowWidth = m_Window->GetWindowSize().X;
        auto windowHeight = m_Window->GetWindowSize().Y;

        if (m_SwapchainWidth == windowWidth || m_SwapchainHeight == windowHeight)
            return;

        glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer);
        glViewport(0, 0, windowWidth, windowHeight);

        m_SwapchainWidth = windowWidth;
        m_SwapchainHeight = windowHeight;
    }

    void SwapchainOpenGL::BindAsRenderTarget()
    {
        SDL_GL_MakeCurrent(m_Window->GetSDLWindowHandle(), m_Context);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer);

        auto width = m_Window->GetWindowSize().X;
        auto height = m_Window->GetWindowSize().Y;
        glViewport(0, 0, width, height);
        glScissor(0, 0, width, height);
    }

    void SwapchainOpenGL::BindAsDrawTarget()
    {
        SDL_GL_MakeCurrent(m_Window->GetSDLWindowHandle(), m_Context);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Backbuffer);
    }

    // static member initialisation
    bool SwapchainOpenGL::s_ContextCreated = false;
    SDL_Window *SwapchainOpenGL::s_ContextWindow = nullptr;

    void SwapchainOpenGL::Prepare()
    {
    }

    bool SwapchainOpenGL::HasContextBeenCreated()
    {
        return s_ContextCreated;
    }
}

#endif