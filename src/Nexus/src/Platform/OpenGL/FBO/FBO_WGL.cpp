#include "FBO_WGL.hpp"

#include "glad/glad_wgl.h"

namespace Nexus::GL
{
	FBO_WGL::FBO_WGL(HWND hwnd, PBufferWGL *pbuffer) : m_HWND(hwnd)
	{
		m_HDC	= GetDC(m_HWND);
		m_HGLRC = CreateSharedContext(m_HDC, pbuffer->GetHGLRC());
		MakeCurrent();
	}

	FBO_WGL::~FBO_WGL()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_HGLRC);
	}

	void FBO_WGL::MakeCurrent()
	{
		wglMakeCurrent(m_HDC, m_HGLRC);
	}

	void FBO_WGL::Swap()
	{
		SwapBuffers(m_HDC);
	}

	void FBO_WGL::SetVSync(bool enabled)
	{
		wglSwapIntervalEXT(enabled);
	}

	HGLRC FBO_WGL::CreateSharedContext(HDC hdc, HGLRC sharedContext)
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

		int	  pixelFormat;
		UINT  numFormats;
		float fAttributes[] = {0, 0};
		int	  iAttributes[] = {WGL_DRAW_TO_WINDOW_ARB,
							   GL_TRUE,
							   WGL_SAMPLE_BUFFERS_ARB,
							   GL_TRUE,
							   WGL_SAMPLES_ARB,
							   8,
							   WGL_DOUBLE_BUFFER_ARB,
							   GL_TRUE,
							   WGL_SAMPLE_BUFFERS_ARB,
							   GL_TRUE,
							   WGL_SAMPLES_ARB,
							   8,
							   WGL_SUPPORT_OPENGL_ARB,
							   GL_TRUE,
							   WGL_PIXEL_TYPE_ARB,
							   WGL_TYPE_RGBA_ARB,
							   WGL_RED_BITS_ARB,
							   8,
							   WGL_GREEN_BITS_ARB,
							   8,
							   WGL_BLUE_BITS_ARB,
							   8,
							   WGL_ALPHA_BITS_ARB,
							   8,
							   WGL_DEPTH_BITS_ARB,
							   24,
							   WGL_STENCIL_BITS_ARB,
							   8,
							   0};

		wglChoosePixelFormatARB(hdc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		int attributes[] =
		{WGL_CONTEXT_MAJOR_VERSION_ARB, 4, WGL_CONTEXT_MINOR_VERSION_ARB, 6, WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0};

		HGLRC hglrc = wglCreateContextAttribsARB(hdc, sharedContext, attributes);

		NX_ASSERT(hglrc, "Failed to create hglrc");

		return hglrc;
	}
}	 // namespace Nexus::GL
