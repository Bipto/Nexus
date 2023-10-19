#include "SwapchainOpenGL.hpp"

#include "GL.hpp"
#include "SDL.h"

namespace Nexus::Graphics
{
    SwapchainOpenGL::SwapchainOpenGL(Window *window, VSyncState vSyncState)
        : m_Window(window), m_VsyncState(vSyncState)
    {
        m_SwapchainWidth = m_Window->GetWindowSize().X;
        m_SwapchainHeight = m_Window->GetWindowSize().Y;

        m_Context = SDL_GL_CreateContext(window->GetSDLWindowHandle());
        SDL_GL_MakeCurrent(window->GetSDLWindowHandle(), m_Context);

        // the backbuffer integer is usually 0, but this cannot be guaranteed, so we check it
        // glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_Backbuffer);
    }

    SwapchainOpenGL::~SwapchainOpenGL()
    {
        SDL_GL_DeleteContext(m_Context);
    }

    void SwapchainOpenGL::SwapBuffers()
    {
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

    void SwapchainOpenGL::Bind()
    {
        SDL_GL_MakeCurrent(m_Window->GetSDLWindowHandle(), m_Context);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer);
    }
}
