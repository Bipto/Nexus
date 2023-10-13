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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);

        m_SwapchainWidth = windowWidth;
        m_SwapchainHeight = windowHeight;
    }
}
