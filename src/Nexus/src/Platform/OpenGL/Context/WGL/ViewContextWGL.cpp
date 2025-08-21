#if defined(NX_PLATFORM_WGL)

	#include "ViewContextWGL.hpp"

	#include "glad/wgl.h"

	#include "Platform/OpenGL/GL.hpp"

namespace Nexus::GL
{
	ViewContextWGL::ViewContextWGL(HWND hwnd, HDC hdc, OffscreenContextWGL *context, const ContextSpecification &spec)
		: m_HWND(hwnd),
		  m_HDC(hdc),
		  m_Description(spec),
		  m_PBuffer(context)
	{
		m_HGLRC = CreateSharedContext(m_HDC, context->GetHGLRC(), spec);
		m_FunctionContext.Load();
	}

	ViewContextWGL::~ViewContextWGL()
	{
		GL::ClearCurrentContext();

		// wglMakeCurrent(NULL, NULL);

		// TODO: Why does this crash sometimes???
		// wglDeleteContext(m_HGLRC);

		if (GL::GetCurrentContext() == this) {}

		m_PBuffer->MakeCurrent();
	}

	void PrintErrorMessage(DWORD error)
	{
		LPSTR  messageBuffer = nullptr;
		size_t size			 = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									  NULL,
									  error,
									  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
									  (LPSTR)&messageBuffer,
									  0,
									  NULL);

		if (size > 0)
		{
			std::cout << "Error: " << messageBuffer << std::endl;
			LocalFree(messageBuffer);
		}
	}

	bool ViewContextWGL::MakeCurrent()
	{
		bool success = wglMakeCurrent(m_HDC, m_HGLRC);

		if (!success)
		{
			DWORD errorCode = GetLastError();
			PrintErrorMessage(errorCode);
		}

		m_FunctionContext.ExecuteCommands([&](const GladGLContext &context) { context.BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); });

		return success;
	}

	void ViewContextWGL::Swap()
	{
		SwapBuffers(m_HDC);
	}

	void ViewContextWGL::SetVSync(bool enabled)
	{
		MakeCurrent();
		wglSwapIntervalEXT(enabled);
	}

	const ContextSpecification &ViewContextWGL::GetDescription() const
	{
		return m_Description;
	}

	HGLRC ViewContextWGL::CreateSharedContext(HDC hdc, HGLRC sharedContext, const ContextSpecification &spec)
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

		if (spec.Samples > 1)
		{
			iAttributes.push_back(WGL_SAMPLE_BUFFERS_ARB);
			iAttributes.push_back(GL_TRUE);
			iAttributes.push_back(WGL_SAMPLES_ARB);
			iAttributes.push_back(spec.Samples);
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
			attributes.push_back(WGL_CONTEXT_PROFILE_MASK_ARB);
			attributes.push_back(WGL_CONTEXT_CORE_PROFILE_BIT_ARB);
		}
		else
		{
			attributes.push_back(WGL_CONTEXT_PROFILE_MASK_ARB);
			attributes.push_back(WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB);
		}

		if (spec.Debug)
		{
			attributes.push_back(WGL_CONTEXT_FLAGS_ARB);
			attributes.push_back(WGL_CONTEXT_DEBUG_BIT_ARB);
		}

		attributes.push_back(0);

		HGLRC hglrc = wglCreateContextAttribsARB(hdc, sharedContext, attributes.data());

		NX_VALIDATE(hglrc, "Failed to create hglrc");

		return hglrc;
	}

	bool ViewContextWGL::Validate()
	{
		return m_HWND != nullptr && m_HDC != nullptr && m_HGLRC != nullptr;
	}

	const GladGLContext &ViewContextWGL::GetContext() const
	{
		return m_FunctionContext.GetContext();
	}
}	 // namespace Nexus::GL

#endif