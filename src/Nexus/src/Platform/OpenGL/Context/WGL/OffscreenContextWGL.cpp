#if defined(NX_PLATFORM_WGL)

	#include "OffscreenContextWGL.hpp"

namespace Nexus::GL
{
	OffscreenContextWGL::OffscreenContextWGL()
	{
		auto [tempWindow, tempHglrc, tempHdc] = CreateTemporaryWindow();
		wglMakeCurrent(tempHdc, tempHglrc);
		LoadGLFunctionsIfNeeded(tempHdc);

		auto [pbuffer, hdc, hglrc] = CreatePBufferContext(tempHdc);

		m_PBuffer = pbuffer;
		m_HDC	  = hdc;
		m_HGLRC	  = hglrc;

		wglMakeCurrent(m_HDC, m_HGLRC);

		wglDeleteContext(tempHglrc);
		ReleaseDC(tempWindow, tempHdc);
		DestroyWindow(tempWindow);
	}

	OffscreenContextWGL::~OffscreenContextWGL()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_HGLRC);
		wglDestroyPbufferARB(m_PBuffer);
	}

	bool OffscreenContextWGL::MakeCurrent()
	{
		return wglMakeCurrent(m_HDC, m_HGLRC);
	}

	HGLRC OffscreenContextWGL::GetHGLRC()
	{
		return m_HGLRC;
	}

	inline void OffscreenContextWGL::LoadGLFunctionsIfNeeded(HDC hdc)
	{
		if (s_GLFunctionsLoaded)
		{
			return;
		}

		if (!gladLoadWGL(hdc))
		{
			MessageBox(NULL, "Failed to load WGL", "Error", MB_OK);
		}

		if (!gladLoadGL())
		{
			MessageBox(NULL, "Failed to load OpenGL function pointers", "Error", MB_OK);
		}

		s_GLFunctionsLoaded = true;
	}

	std::tuple<HWND, HGLRC, HDC> OffscreenContextWGL::CreateTemporaryWindow()
	{
		const char className[] = "Temporary OpenGL Window";

		WNDCLASS wc		 = {};
		wc.lpfnWndProc	 = DefWindowProc;
		wc.hInstance	 = NULL;
		wc.lpszClassName = className;
		wc.style		 = CS_OWNDC;
		RegisterClass(&wc);

		HWND hwnd = CreateWindowEx(0,
								   className,
								   "TempWindow",
								   WS_OVERLAPPEDWINDOW,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   NULL,
								   NULL,
								   NULL,
								   NULL);

		if (!IsWindow(hwnd))
		{
			MessageBox(NULL, "Could not create window", "Error", MB_OK);
		}

		ShowWindow(hwnd, SW_HIDE);

		HDC hdc = GetDC(hwnd);

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

		int pixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		HGLRC hglrc = wglCreateContext(hdc);

		NX_ASSERT(hglrc, "Failed to create HGLRC");

		return {hwnd, hglrc, hdc};
	}

	std::tuple<HPBUFFERARB, HDC, HGLRC> OffscreenContextWGL::CreatePBufferContext(HDC hdc)
	{
		int	  pixelFormat;
		UINT  numFormats;
		float fAttributes[] = {0, 0};
		int	  iAttributes[] = {WGL_DRAW_TO_PBUFFER_ARB,
							   GL_TRUE,
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
							   0};

		wglChoosePixelFormatARB(hdc, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

		HPBUFFERARB pbuffer	  = wglCreatePbufferARB(hdc, pixelFormat, 1, 1, NULL);
		HDC			pbufferDC = wglGetPbufferDCARB(pbuffer);
		HGLRC		hglrc	  = wglCreateContext(pbufferDC);

		return {pbuffer, pbufferDC, hglrc};
	}
}	 // namespace Nexus::GL

#endif