#include "SwapchainOpenGL.hpp"

#include "GL.hpp"

namespace Nexus::Graphics
{
    SwapchainOpenGL::SwapchainOpenGL(SDL_Window *window, VSyncState vSyncState)
    {
        m_Window = window;
        m_VsyncState = vSyncState;
    }

    void SwapchainOpenGL::SwapBuffers()
    {
        SDL_GL_SwapWindow(m_Window);
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

    void SwapchainOpenGL::Resize(uint32_t width, uint32_t height)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
    }
}
