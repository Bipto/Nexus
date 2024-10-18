#include "ContextWGL.hpp"

#include "Nexus-Core/Logging/Log.hpp"

#include "wingdi.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::string GetErrorMessage(DWORD error)
{
	LPVOID lpMsgBuf;
	DWORD  bufLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								  NULL,
								  error,
								  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								  (LPSTR)&lpMsgBuf,
								  0,
								  NULL);

	if (bufLen)
	{
		std::string message((LPSTR)lpMsgBuf, bufLen);
		LocalFree(lpMsgBuf);
		return message;
	}
	return "Unknown error";
}

namespace Nexus::GL
{
	ContextWGL::ContextWGL(uint32_t width, uint32_t height) : m_ContextSource(ContextSource::PBuffer)
	{
		LoadOpenGLIfNeeded();
		CreatePbufferContext(width, height);
	}

	ContextWGL::ContextWGL(HWND hwnd) : m_ContextSource(ContextSource::Swapchain)
	{
		LoadOpenGLIfNeeded();
	}

	ContextWGL::~ContextWGL()
	{
	}

	void ContextWGL::Swap()
	{
		MakeCurrent();

		HDC hdc = wglGetCurrentDC();
		SwapBuffers(hdc);
	}

	void ContextWGL::MakeCurrent()
	{
		wglMakeCurrent(m_PBufferHDC, m_HGLRC);
	}

	ContextSource ContextWGL::GetSource()
	{
		return m_ContextSource;
	}

	void ContextWGL::LoadOpenGLIfNeeded()
	{
		if (s_FunctionsLoaded)
		{
			return;
		}

		const char className[] = "Sample Window Class";

		WNDCLASS wc		 = {};
		wc.lpfnWndProc	 = WindowProc;
		wc.hInstance	 = NULL;
		wc.lpszClassName = className;
		wc.style		 = CS_OWNDC;

		if (!RegisterClass(&wc))
		{
			DWORD		error		 = GetLastError();
			std::string errorMessage = GetErrorMessage(error);
			MessageBox(NULL, errorMessage.c_str(), "Error", MB_OK);
			return;
		}

		s_HWND = CreateWindowEx(0,
								className,
								"My Window",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								NULL,
								NULL,
								NULL,
								NULL);

		if (!IsWindow(s_HWND))
		{
			DWORD		error		 = GetLastError();
			std::string errorMessage = GetErrorMessage(error);
			MessageBox(NULL, errorMessage.c_str(), "Error", MB_OK);
			return;
		}

		ShowWindow(s_HWND, SW_HIDE);

		s_HDC = GetDC(s_HWND);

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

		int windowPixelFormat = ChoosePixelFormat(s_HDC, &pfd);
		SetPixelFormat(s_HDC, windowPixelFormat, &pfd);

		HGLRC tempContext = wglCreateContext(s_HDC);
		wglMakeCurrent(s_HDC, tempContext);

		if (!gladLoadWGL(s_HDC))
		{
			MessageBox(NULL, "Failed to load WGL", "Error", MB_OK);
		}

		if (!gladLoadGL())
		{
			MessageBox(NULL, "Failed to load OpenGL functions", "Error", MB_OK);
		}
	}

	void ContextWGL::CreatePbufferContext(uint32_t width, uint32_t height)
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

		wglChoosePixelFormatARB(s_HDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

		m_PBuffer	 = wglCreatePbufferARB(s_HDC, pixelFormat, width, height, NULL);
		m_PBufferHDC = wglGetPbufferDCARB(m_PBuffer);

		m_HGLRC = wglCreateContext(m_PBufferHDC);
		MakeCurrent();
	}
}	 // namespace Nexus::GL