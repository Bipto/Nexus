#if defined(NX_PLATFORM_EGL)

	#include "OffscreenContextEGL.hpp"

	#include "glad/glad.h"

	#include "Platform/X11/X11Include.hpp"

	#include "EGLUtils.hpp"

namespace Nexus::GL
{
	OffscreenContextEGL::OffscreenContextEGL(EGLDisplay display, const ContextSpecification &spec) : m_EGLDisplay(display), m_Specification(spec)

	{
		if (m_EGLDisplay == EGL_NO_DISPLAY)
		{
			std::cout << "Failed to get EGL display: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}

		EGLint major = 0;
		EGLint minor = 0;
		if (!eglInitialize(m_EGLDisplay, &major, &minor))
		{
			std::cout << "Failed to initialize EGL: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}

		eglBindAPI(EGL_OPENGL_API);

		EGLint configAttribs[] = {EGL_SURFACE_TYPE,
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
								  EGL_NONE};

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

		m_Context = eglCreateContext(m_EGLDisplay, config, EGL_NO_CONTEXT, contextAttribs);
		if (m_Context == EGL_NO_CONTEXT)
		{
			std::cout << "Failed to create EGL context: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}

		if (!MakeCurrent())
		{
			std::cout << "Could not make context current: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}

		if (!gladLoadEGL())
		{
			std::cout << "Failed to load EGL functions" << std::endl;
		}

		if (!gladLoadGLLoader((GLADloadproc)eglGetProcAddress))
		{
			std::cout << "Failed to load OpenGL functions" << std::endl;
		}
	}

	OffscreenContextEGL::~OffscreenContextEGL()
	{
		eglDestroyContext(m_EGLDisplay, m_Context);
		eglTerminate(m_EGLDisplay);
	}

	bool OffscreenContextEGL::MakeCurrent()
	{
		return eglMakeCurrent(m_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, m_Context);
	}

	EGLContext OffscreenContextEGL::GetContext()
	{
		return m_Context;
	}

}	 // namespace Nexus::GL

#endif