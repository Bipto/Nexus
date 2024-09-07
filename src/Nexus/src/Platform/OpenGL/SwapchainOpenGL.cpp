#if defined(NX_PLATFORM_OPENGL)

#include "SwapchainOpenGL.hpp"

#include "GL.hpp"
#include "SDL.h"

#include "BufferOpenGL.hpp"

namespace Nexus::Graphics {
SwapchainOpenGL::SwapchainOpenGL(Window *window,
                                 const SwapchainSpecification &swapchainSpec)
    : Swapchain(swapchainSpec), m_Window(window),
      m_VsyncState(swapchainSpec.VSyncState) {
  m_SwapchainWidth = m_Window->GetWindowSize().X;
  m_SwapchainHeight = m_Window->GetWindowSize().Y;

  if (!s_ContextWindow) {
    s_ContextWindow = window->GetSDLWindowHandle();
  }

  m_Context = SDL_GL_CreateContext(s_ContextWindow);

  SDL_GL_MakeCurrent(window->GetSDLWindowHandle(), m_Context);

  if (SDL_GL_MakeCurrent(window->GetSDLWindowHandle(), m_Context) != 0) {
    std::string error = SDL_GetError();
    throw std::runtime_error(error);
  }
}

SwapchainOpenGL::~SwapchainOpenGL() {}

void SwapchainOpenGL::SwapBuffers() {
  BindAsRenderTarget();

  SDL_GL_SwapWindow(m_Window->GetSDLWindowHandle());
  ResizeIfNecessary();
}

VSyncState SwapchainOpenGL::GetVsyncState() { return m_VsyncState; }

void SwapchainOpenGL::SetVSyncState(VSyncState vsyncState) {
  m_VsyncState = vsyncState;
  SDL_GL_SetSwapInterval((unsigned int)m_VsyncState);
}

Nexus::Point2D<uint32_t> SwapchainOpenGL::GetSize() {
  return {m_SwapchainWidth, m_SwapchainHeight};
}

void SwapchainOpenGL::ResizeIfNecessary() {
  int w, h;
  SDL_GetWindowSizeInPixels(m_Window->GetSDLWindowHandle(), &w, &h);

  glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer));
  glCall(glViewport(0, 0, w, h));
  glCall(glScissor(0, 0, w, h));

  m_SwapchainWidth = w;
  m_SwapchainHeight = h;
}

void SwapchainOpenGL::BindAsRenderTarget() {
  if (SDL_GL_MakeCurrent(m_Window->GetSDLWindowHandle(), m_Context) != 0) {
    std::string error = SDL_GetError();
    throw std::runtime_error(error);
  }

  glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Backbuffer));
  ResizeIfNecessary();
}

void SwapchainOpenGL::BindAsDrawTarget() {
  if (SDL_GL_MakeCurrent(m_Window->GetSDLWindowHandle(), m_Context) != 0) {
    std::string error = SDL_GetError();
    throw std::runtime_error(error);
  }

  glCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Backbuffer));
}

// static member initialisation
SDL_Window *SwapchainOpenGL::s_ContextWindow = nullptr;

void SwapchainOpenGL::Prepare() {}

bool SwapchainOpenGL::HasContextBeenCreated() { return s_ContextWindow; }
} // namespace Nexus::Graphics

#endif