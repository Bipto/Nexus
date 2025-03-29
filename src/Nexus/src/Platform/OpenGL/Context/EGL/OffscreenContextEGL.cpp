#if defined(NX_PLATFORM_EGL)

	#include "OffscreenContextEGL.hpp"

	#include "glad/glad.h"

	#if defined(NX_PLATFORM_LINUX)
		#include "Platform/X11/X11Include.hpp"
	#endif

	#include "EGLUtils.hpp"

namespace Nexus::GL
{
	OffscreenContextEGL::OffscreenContextEGL(EGLDisplay display, const ContextSpecification &spec) : m_EGLDisplay(display), m_Specification(spec)

	{
		if (m_EGLDisplay == EGL_NO_DISPLAY)
		{
			std::cout << "Failed to get EGL display: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}

		EGLint major = 0;
		EGLint minor = 0;
		if (!eglInitialize(m_EGLDisplay, &major, &minor))
		{
			std::cout << "Failed to initialize EGL: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}

		if (spec.GLVersion == GL::OpenGLVersion::OpenGL)
		{
			eglBindAPI(EGL_OPENGL_API);
		}
		else
		{
			eglBindAPI(EGL_OPENGL_ES_API);
		}

		std::vector<EGLint> configAttribs;
		configAttribs.push_back(EGL_SURFACE_TYPE);
		configAttribs.push_back(EGL_PBUFFER_BIT);
		configAttribs.push_back(EGL_RENDERABLE_TYPE);

		if (spec.GLVersion == GL::OpenGLVersion::OpenGL)
		{
			configAttribs.push_back(EGL_OPENGL_BIT);
		}
		else
		{
			configAttribs.push_back(EGL_OPENGL_ES3_BIT);
		}

		configAttribs.push_back(EGL_RED_SIZE);
		configAttribs.push_back(spec.RedBits);
		configAttribs.push_back(EGL_GREEN_SIZE);
		configAttribs.push_back(spec.GreenBits);
		configAttribs.push_back(EGL_BLUE_SIZE);
		configAttribs.push_back(spec.BlueBits);
		configAttribs.push_back(EGL_ALPHA_SIZE);
		configAttribs.push_back(spec.AlphaBits);
		configAttribs.push_back(EGL_DEPTH_SIZE);
		configAttribs.push_back(spec.DepthBits);
		configAttribs.push_back(EGL_STENCIL_SIZE);
		configAttribs.push_back(spec.StencilBits);
		configAttribs.push_back(EGL_NONE);

		EGLConfig config;
		EGLint	  numConfigs;
		if (!eglChooseConfig(m_EGLDisplay, configAttribs.data(), &config, 1, &numConfigs))
		{
			std::cout << "Failed to choose EGL config: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}

		std::vector<EGLint> contextAttribs;
		/*contextAttribs.push_back(EGL_CONTEXT_MAJOR_VERSION);
		contextAttribs.push_back(spec.VersionMajor);
		contextAttribs.push_back(EGL_CONTEXT_MINOR_VERSION);
		contextAttribs.push_back(spec.VersionMinor);

		if (spec.GLVersion == GL::OpenGLVersion::OpenGL)
		{
			if (spec.UseCoreProfile)
			{
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_PROFILE_MASK);
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT);
			}
			else
			{
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_PROFILE_MASK);
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT);
			}

			if (spec.Debug)
			{
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_DEBUG);
				contextAttribs.push_back(EGL_TRUE);
			}
		} */

		contextAttribs.push_back(EGL_CONTEXT_CLIENT_VERSION);
		contextAttribs.push_back(3);
		contextAttribs.push_back(EGL_NONE);

		m_Context = eglCreateContext(m_EGLDisplay, config, EGL_NO_CONTEXT, contextAttribs.data());
		if (m_Context == EGL_NO_CONTEXT)
		{
			std::cout << "Failed to create EGL context: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}

		if (!MakeCurrent())
		{
			std::cout << "Could not make context current: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
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

	bool OffscreenContextEGL::Validate()
	{
		return m_EGLDisplay != 0 && m_Context != 0;
	}

}	 // namespace Nexus::GL

#endif