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
		std::vector<EGLint> configAttribs;
		configAttribs.push_back(EGL_SURFACE_TYPE);
		configAttribs.push_back(EGL_WINDOW_BIT);
		configAttribs.push_back(EGL_RENDERABLE_TYPE);
		configAttribs.push_back(EGL_OPENGL_BIT);
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

		if (spec.Samples != Graphics::SampleCount::SampleCount1)
		{
			uint32_t samples = Graphics::GetSampleCount(spec.Samples);
			configAttribs.push_back(EGL_SAMPLE_BUFFERS);
			configAttribs.push_back(EGL_TRUE);
			configAttribs.push_back(EGL_SAMPLES);
			configAttribs.push_back(samples);
		}

		configAttribs.push_back(EGL_NONE);

		if (m_EGLDisplay == EGL_NO_DISPLAY)
		{
			std::cout << "Failed to choose EGL config: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}
		eglInitialize(m_EGLDisplay, NULL, NULL);

		eglBindAPI(EGL_OPENGL_API);

		EGLConfig config;
		EGLint	  numConfigs;
		if (!eglChooseConfig(m_EGLDisplay, configAttribs.data(), &config, 1, &numConfigs))
		{
			std::cout << "Failed to choose EGL config: ";
			std::cout << eglGetErrorString(eglGetError()) << std::endl;
		}

		std::vector<EGLint> contextAttribs;
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

		if (spec.Debug)
		{
			contextAttribs.push_back(EGL_CONTEXT_OPENGL_DEBUG);
			contextAttribs.push_back(EGL_TRUE);
		}

		contextAttribs.push_back(EGL_NONE);

		m_Context = eglCreateContext(m_EGLDisplay, config, pbuffer->GetContext(), contextAttribs.data());
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

		SetVSync(spec.Vsync);
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
		eglSwapInterval(m_EGLDisplay, (EGLint)enabled);
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