#if defined(NX_PLATFORM_WGL)

	#include "FBO_WGL.hpp"

	#include "glad/glad_wgl.h"

	#include "Nexus-Core/Graphics/Multisample.hpp"

namespace Nexus::GL
{
	FBO_WGL::FBO_WGL(HWND hwnd, PBufferWGL *pbuffer, const ContextSpecification &spec) : m_HWND(hwnd), m_Specification(spec)
	{
		m_HDC	= GetDC(m_HWND);
		m_HGLRC = CreateSharedContext(m_HDC, pbuffer->GetHGLRC(), spec);
		MakeCurrent();
	}

	FBO_WGL::~FBO_WGL()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_HGLRC);
	}

	bool FBO_WGL::MakeCurrent()
	{
		return wglMakeCurrent(m_HDC, m_HGLRC);
	}

	void FBO_WGL::Swap()
	{
		MakeCurrent();
		SwapBuffers(m_HDC);
	}

	void FBO_WGL::SetVSync(bool enabled)
	{
		MakeCurrent();
		wglSwapIntervalEXT(enabled);
	}

	const ContextSpecification &FBO_WGL::GetSpecification() const
	{
		return m_Specification;
	}

	HGLRC FBO_WGL::CreateSharedContext(HDC hdc, HGLRC sharedContext, const ContextSpecification &spec)
	{
		PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
									 1,
									 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
									 PFD_TYPE_RGBA,
									 32,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 0,
									 24,
									 8,
									 0,
									 PFD_MAIN_PLANE,
									 0,
									 0,
									 0,
									 0};

		int				 pixelFormat;
		UINT			 numFormats;
		float			 fAttributes[] = {0, 0};
		std::vector<int> iAttributes   = {};

		iAttributes.push_back(WGL_DRAW_TO_WINDOW_ARB);
		iAttributes.push_back(GL_TRUE);

		if (spec.Samples != Graphics::SampleCount::SampleCount1)
		{
			uint32_t samples = Graphics::GetSampleCount(spec.Samples);
			iAttributes.push_back(WGL_SAMPLE_BUFFERS_ARB);
			iAttributes.push_back(GL_TRUE);
			iAttributes.push_back(WGL_SAMPLES_ARB);
			iAttributes.push_back(samples);
		}

		if (spec.DoubleBuffered)
		{
			iAttributes.push_back(WGL_DOUBLE_BUFFER_ARB);
			iAttributes.push_back(GL_TRUE);
		}
		iAttributes.push_back(WGL_SUPPORT_OPENGL_ARB);
		iAttributes.push_back(GL_TRUE);
		iAttributes.push_back(WGL_PIXEL_TYPE_ARB);
		iAttributes.push_back(WGL_TYPE_RGBA_ARB);

		iAttributes.push_back(WGL_RED_BITS_ARB);
		iAttributes.push_back(spec.RedBits);
		iAttributes.push_back(WGL_GREEN_BITS_ARB);
		iAttributes.push_back(spec.GreenBits);
		iAttributes.push_back(WGL_BLUE_BITS_ARB);
		iAttributes.push_back(spec.BlueBits);
		iAttributes.push_back(WGL_ALPHA_BITS_ARB);
		iAttributes.push_back(spec.AlphaBits);
		iAttributes.push_back(WGL_DEPTH_BITS_ARB);
		iAttributes.push_back(spec.DepthBits);
		iAttributes.push_back(WGL_STENCIL_BITS_ARB);
		iAttributes.push_back(spec.StencilBits);
		iAttributes.push_back(0);

		wglChoosePixelFormatARB(hdc, iAttributes.data(), fAttributes, 1, &pixelFormat, &numFormats);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		std::vector<int> attributes = {};
		attributes.push_back(WGL_CONTEXT_MAJOR_VERSION_ARB);
		attributes.push_back(spec.VersionMajor);
		attributes.push_back(WGL_CONTEXT_MINOR_VERSION_ARB);
		attributes.push_back(spec.VersionMinor);

		if (spec.UseCoreProfile)
		{
			attributes.push_back(WGL_CONTEXT_FLAGS_ARB);
			attributes.push_back(WGL_CONTEXT_CORE_PROFILE_BIT_ARB);
		}
		else
		{
			attributes.push_back(WGL_CONTEXT_FLAGS_ARB);
			attributes.push_back(WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB);
		}

		if (spec.Debug)
		{
			attributes.push_back(WGL_CONTEXT_FLAGS_ARB);
			attributes.push_back(WGL_CONTEXT_DEBUG_BIT_ARB);
		}

		attributes.push_back(0);

		HGLRC hglrc = wglCreateContextAttribsARB(hdc, sharedContext, attributes.data());

		NX_ASSERT(hglrc, "Failed to create hglrc");

		return hglrc;
	}
}	 // namespace Nexus::GL

#endif