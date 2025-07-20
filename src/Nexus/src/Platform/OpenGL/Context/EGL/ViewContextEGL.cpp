#if defined(NX_PLATFORM_EGL)

	#include "ViewContextEGL.hpp"

	#include "glad/gl.h"

	#include "Platform/OpenGL/GL.hpp"

	#include "EGLUtils.hpp"

namespace Nexus::GL
{
	ViewContextEGL::ViewContextEGL(EGLDisplay display, EGLNativeWindowType window, OffscreenContextEGL *pbuffer, const ContextSpecification &spec)
		: m_EGLDisplay(display),
		  m_PBuffer(pbuffer),
		  m_Description(spec)
	{
		std::vector<EGLint> configAttribs;
		configAttribs.push_back(EGL_SURFACE_TYPE);
		configAttribs.push_back(EGL_WINDOW_BIT);
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

		if (spec.Samples != 1)
		{
			configAttribs.push_back(EGL_SAMPLE_BUFFERS);
			configAttribs.push_back(1);
			configAttribs.push_back(EGL_SAMPLES);
			configAttribs.push_back(spec.Samples);
		}

		configAttribs.push_back(EGL_NONE);

		if (m_EGLDisplay == EGL_NO_DISPLAY)
		{
			std::cout << "Failed to choose EGL config: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}
		eglInitialize(m_EGLDisplay, NULL, NULL);

		if (spec.GLVersion == GL::OpenGLVersion::OpenGL)
		{
			eglBindAPI(EGL_OPENGL_API);
		}
		else
		{
			eglBindAPI(EGL_OPENGL_ES_API);
		}

		EGLConfig config;
		EGLint	  numConfigs;
		if (!eglChooseConfig(m_EGLDisplay, configAttribs.data(), &config, 1, &numConfigs))
		{
			std::cout << "Failed to choose EGL config: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}

		std::vector<EGLint> contextAttribs;
		if (spec.GLVersion == GL::OpenGLVersion::OpenGL)
		{
			contextAttribs.push_back(EGL_CONTEXT_MAJOR_VERSION);
			contextAttribs.push_back(spec.VersionMajor);
			contextAttribs.push_back(EGL_CONTEXT_MINOR_VERSION);
			contextAttribs.push_back(spec.VersionMinor);

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
		}
		else
		{
			contextAttribs.push_back(EGL_CONTEXT_CLIENT_VERSION);
			contextAttribs.push_back(spec.VersionMajor);
			contextAttribs.push_back(EGL_NONE);
		}

		if (spec.Debug)
		{
			contextAttribs.push_back(EGL_CONTEXT_OPENGL_DEBUG);
			contextAttribs.push_back(EGL_TRUE);
		}

		contextAttribs.push_back(EGL_NONE);

		m_Context = eglCreateContext(m_EGLDisplay, config, pbuffer->GetEGLContext(), contextAttribs.data());
		if (m_Context == EGL_NO_CONTEXT)
		{
			std::cout << "Failed to create OpenGL context: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}

		EGLAttrib attr[] = {EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR, EGL_RENDER_BUFFER, EGL_BACK_BUFFER, EGL_NONE};

		m_Surface = eglCreateWindowSurface(m_EGLDisplay, config, window, NULL);
		if (m_Surface == EGL_NO_SURFACE)
		{
			std::cout << "Failed to create window surface: ";
			const char *errorMessage = eglGetErrorString(eglGetError());
			std::cout << errorMessage << std::endl;
		}

        if (!eglMakeCurrent(m_EGLDisplay, m_Surface, m_Surface, m_Context))
        {
            std::cout << "Failed to make context current" << std::endl;
        }

        eglSwapInterval(m_EGLDisplay, (EGLint)spec.Vsync);

        if (spec.GLVersion == OpenGLVersion::OpenGL)
        {
            if (!gladLoaderLoadGLContext(&m_GladContext))
            {
                std::cout << "Failed to load OpenGL function pointers" << std::endl;
            }

        }
        else
        {
            if (!gladLoadGLES2Context(&m_GladContext, (GLADloadfunc)eglGetProcAddress))
            {
                std::cout << "Failed to load OpenGLES function pointers" << std::endl;
            }
        }
	}

	ViewContextEGL::~ViewContextEGL()
	{
		GL::ClearCurrentContext();
		gladLoaderUnloadGLContext(&m_GladContext);
		eglDestroyContext(m_EGLDisplay, m_Context);
	}

	bool ViewContextEGL::MakeCurrent()
	{
		return eglMakeCurrent(m_EGLDisplay, m_Surface, m_Surface, m_Context);
	}

	void ViewContextEGL::Swap()
	{
		GL::SetCurrentContext(this);
		eglSwapBuffers(m_EGLDisplay, m_Surface);
	}

	void ViewContextEGL::SetVSync(bool enabled)
	{
		eglSwapInterval(m_EGLDisplay, (EGLint)enabled);
	}

	const ContextSpecification &ViewContextEGL::GetDescription() const
	{
		return m_Description;
	}

	bool ViewContextEGL::Validate()
	{
		return m_EGLDisplay != 0 && m_NativeWindow != 0 && m_Surface != 0 && m_Context != 0;
	}

	const GladGLContext &ViewContextEGL::GetContext() const
	{
		return m_GladContext;
	}
}	 // namespace Nexus::GL

#endif