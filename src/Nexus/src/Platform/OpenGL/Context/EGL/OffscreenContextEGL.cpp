#if defined(NX_PLATFORM_EGL)

	#include "OffscreenContextEGL.hpp"

	#include "glad/gl.h"

	#if defined(NX_PLATFORM_LINUX)
		#include "Platform/X11/X11Include.hpp"
	#endif

	#include "EGLUtils.hpp"

	#include "Platform/OpenGL/GL.hpp"

	#include "Nexus-Core/Platform.hpp"

namespace Nexus::GL
{
	OffscreenContextEGL::OffscreenContextEGL(const ContextSpecification &spec, EGLDisplay display) : m_Description(spec)

	{
		m_EGLDisplay = display;

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

		const char *apis = eglQueryString(m_EGLDisplay, EGL_CLIENT_APIS);

		int version = gladLoadEGL(m_EGLDisplay, eglGetProcAddress);

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

		if (EGL::HasExtension(m_EGLDisplay, "EGL_KHR_create_context"))
		{
			if (spec.GLVersion == GL::OpenGLVersion::OpenGL)
			{
				contextAttribs.push_back(EGL_CONTEXT_MAJOR_VERSION_KHR);
				contextAttribs.push_back(spec.VersionMajor);
				contextAttribs.push_back(EGL_CONTEXT_MINOR_VERSION_KHR);
				contextAttribs.push_back(spec.VersionMinor);
			}
			else
			{
				contextAttribs.push_back(EGL_CONTEXT_CLIENT_VERSION);
				contextAttribs.push_back(spec.VersionMajor);
			}

			contextAttribs.push_back(EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR);
			if (spec.UseCoreProfile)
			{
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT);
			}
			else
			{
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT);
			}

			if (spec.Debug)
			{
				contextAttribs.push_back(EGL_CONTEXT_FLAGS_KHR);
				contextAttribs.push_back(EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR);
			}
			else
			{
				contextAttribs.push_back(EGL_CONTEXT_FLAGS_KHR);
				contextAttribs.push_back(0);
			}
		}

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

		gladLoaderLoadGLContext(&m_GladContext);
	}

	OffscreenContextEGL::~OffscreenContextEGL()
	{
		GL::ClearCurrentContext();
		gladLoaderUnloadGLContext(&m_GladContext);
		eglDestroyContext(m_EGLDisplay, m_Context);
		eglTerminate(m_EGLDisplay);
	}

	bool OffscreenContextEGL::MakeCurrent()
	{
		return eglMakeCurrent(m_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, m_Context);
	}

	EGLContext OffscreenContextEGL::GetEGLContext()
	{
		return m_Context;
	}

	bool OffscreenContextEGL::Validate()
	{
		return m_EGLDisplay != 0 && m_Context != 0;
	}

	const GladGLContext &OffscreenContextEGL::GetContext() const
	{
		return m_GladContext;
	}

}	 // namespace Nexus::GL

#endif