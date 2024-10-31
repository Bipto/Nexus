#if defined(NX_PLATFORM_EGL)

	#include "PBufferEGL.hpp"

namespace Nexus::GL
{
	Nexus::GL::PBufferEGL::PBufferEGL()
	{
		m_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		NX_ASSERT(m_Display, "Could not open display");

		bool initialised = eglInitialize(m_Display, NULL, NULL);
		NX_ASSERT(initialised, "Could not initialise EGL");

		EGLConfig eglConfig;
		EGLint	  numConfigs;
		EGLint	  configAttribs[] = {EGL_SURFACE_TYPE,
									 EGL_PBUFFER_BIT,
									 EGL_RENDERABLE_TYPE,
									 EGL_OPENGL_BIT,
									 EGL_RED_SIZE,
									 8,
									 EGL_GREEN_SIZE,
									 8,
									 EGL_BLUE_SIZE,
									 8,
									 EGL_ALPHA_SIZE,
									 8,
									 EGL_DEPTH_SIZE,
									 24,
									 EGL_STENCIL_SIZE,
									 8,
									 EGL_NONE};

		bool configChosen = eglChooseConfig(m_Display, configAttribs, &eglConfig, 1, &numConfigs);

		EGLint pbufferAttribs[] = {EGL_WIDTH, 800, EGL_HEIGHT, 800, EGL_NONE};
		m_Surface				= eglCreatePbufferSurface(m_Surface, eglConfig, pbufferAttribs);
		NX_ASSERT(m_Surface, "Could not create EGL surface");

		m_Context = eglCreateContext(m_Display, eglConfig, EGL_NO_CONTEXT, NULL);
		NX_ASSERT(m_Context, "Could not create EGL context");
	}

	PBufferEGL::~PBufferEGL()
	{
		eglDestroySurface(m_Display, m_Surface);
		eglDestroyContext(m_Display, m_Context);
		eglTerminate(m_Display);
	}

	bool PBufferEGL::MakeCurrent()
	{
		return eglMakeCurrent(m_Display, m_Surface, m_Display, m_Context);
	}
}	 // namespace Nexus::GL

#endif