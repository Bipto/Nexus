#if defined(NX_PLATFORM_EGL)

	#include "ViewContextEGL.hpp"

	#include "glad/glad.h"
	#include "glad/glad_egl.h"

	#include "EGLUtils.hpp"

namespace Nexus::GL
{
	ViewContextEGL::ViewContextEGL(EGLDisplay display, EGLNativeWindowType window, OffscreenContextEGL *pbuffer, const ContextSpecification &spec)
		: m_EGLDisplay(display),
		  m_PBuffer(pbuffer),
		  m_Specification(spec)
	{
		EGLint configAttribs[] = {EGL_SURFACE_TYPE,
								  EGL_WINDOW_BIT,
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
								  EGL_NONE};

		if (m_EGLDisplay == EGL_NO_DISPLAY)
		{
			std::cout << "Failed to choose EGL config: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}
		eglInitialize(m_EGLDisplay, NULL, NULL);

		eglBindAPI(EGL_OPENGL_API);

		EGLConfig config;
		EGLint	  numConfigs;
		if (!eglChooseConfig(m_EGLDisplay, configAttribs, &config, 1, &numConfigs))
		{
			std::cout << "Failed to choose EGL config: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}

		EGLint contextAttribs[] = {EGL_CONTEXT_MAJOR_VERSION,
								   4,
								   EGL_CONTEXT_MINOR_VERSION,
								   6,
								   EGL_CONTEXT_OPENGL_PROFILE_MASK,
								   EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
								   EGL_CONTEXT_CLIENT_VERSION,
								   4,
								   EGL_NONE};

		m_Context = eglCreateContext(m_EGLDisplay, config, pbuffer->GetContext(), contextAttribs);
		if (m_Context == EGL_NO_CONTEXT)
		{
			std::cout << "Failed to create OpenGL context: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}

		EGLAttrib attr[] = {EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR, EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL_NONE};

		m_Surface = eglCreateWindowSurface(m_EGLDisplay, config, window, NULL);
		if (m_Surface == EGL_NO_SURFACE)
		{
			std::cout << "Failed to create window surface: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}
	}

	ViewContextEGL::~ViewContextEGL()
	{
	}

	bool ViewContextEGL::MakeCurrent()
	{
		return eglMakeCurrent(m_EGLDisplay, m_Surface, m_Surface, m_Context);
	}

	void ViewContextEGL::Swap()
	{
		eglSwapBuffers(m_EGLDisplay, m_Surface);
	}

	void ViewContextEGL::SetVSync(bool enabled)
	{
	}

	const ContextSpecification &ViewContextEGL::GetSpecification() const
	{
		return m_Specification;
	}

	bool ViewContextEGL::Validate()
	{
		return m_EGLDisplay != 0 && m_NativeWindow != 0 && m_Surface != 0 && m_Context != 0;
	}
}	 // namespace Nexus::GL

#endif