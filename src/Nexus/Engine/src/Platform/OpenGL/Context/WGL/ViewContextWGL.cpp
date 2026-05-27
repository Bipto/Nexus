#include "ViewContextWGL.hpp"

#include "glad/wgl.h"

#include "Platform/OpenGL/GL.hpp"
#include "Platform/OpenGL/TextureOpenGL.hpp"

namespace Nexus::GL
{
	ViewContextWGL::ViewContextWGL(HWND hwnd, HDC hdc, OffscreenContextWGL *context, const ContextDescription &spec)
		: m_HWND(hwnd),
		  m_HDC(hdc),
		  m_Description(spec),
		  m_PBuffer(context)
	{
		m_HGLRC = CreateSharedContext(m_HDC, context->GetHGLRC(), spec);
		wglMakeCurrent(m_HDC, m_HGLRC);
		int result = gladLoaderLoadGLContext(&m_FunctionContext);
	}

	ViewContextWGL::~ViewContextWGL()
	{
		gladLoaderUnloadGLContext(&m_FunctionContext);

		// clear the current context
		wglMakeCurrent(NULL, NULL);

		// delete the context
		wglDeleteContext(m_HGLRC);
	}

	static void PrintErrorMessage(DWORD error)
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
		HGLRC currentContext = wglGetCurrentContext();
		HDC	  currentHDC	 = wglGetCurrentDC();

		// the context is already current, so we can skip this step
		if (currentContext == m_HGLRC && currentHDC == m_HDC)
		{
			return true;
		}

		bool success = wglMakeCurrent(m_HDC, m_HGLRC);

		if (!success)
		{
			DWORD errorCode = GetLastError();
			PrintErrorMessage(errorCode);
		}

		m_FunctionContext.BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return success;
	}

	void ViewContextWGL::Swap(Graphics::TextureHandle texture, const Graphics::SwapchainPresentDescription &presentDesc)
	{
		NX_VALIDATE(texture.IsValid(), "Texture cannot be null");

		Execute(
			[&](const GladGLContext &context)
			{
				// copy the sections requested
				if (presentDesc.PresentRects.size() > 0)
				{
					for (const auto &rect : presentDesc.PresentRects)
					{
						Graphics::TextureCopyDescription copyDesc = {};

						// framebuffer texture
						copyDesc.Source			= texture;
						copyDesc.SourceOffset	= {static_cast<int32_t>(rect.X), static_cast<int32_t>(rect.Y), 0};
						copyDesc.SourceMipLevel = 0;

						// backbuffer
						copyDesc.Destination		 = {};
						copyDesc.DestinationMipLevel = 0;
						copyDesc.DestinationOffset	 = {static_cast<int32_t>(rect.X), static_cast<int32_t>(rect.Y), 0};

						copyDesc.Extent = {rect.Width, rect.Height};
						GL::CopyTextureToTexture(copyDesc, context);
					}
				}
				// copy the full image
				else
				{
					Graphics::TextureCopyDescription copyDesc = {};

					// framebuffer texture
					copyDesc.Source			= texture;
					copyDesc.SourceOffset	= {0, 0, 0};
					copyDesc.SourceMipLevel = 0;

					// backbuffer
					copyDesc.Destination		 = {};
					copyDesc.DestinationMipLevel = 0;
					copyDesc.DestinationOffset	 = {0, 0, 0};
					copyDesc.Extent				 = {texture->GetWidth(), texture->GetHeight()};
					GL::CopyTextureToTexture(copyDesc, context);
				}
			});

		if (wglSwapLayerBuffers != nullptr)
		{
			wglSwapLayerBuffers(m_HDC, WGL_SWAP_MAIN_PLANE);
		}
		else
		{
			SwapBuffers(m_HDC);
		}
	}

	void ViewContextWGL::SetVSync(bool enabled)
	{
		MakeCurrent();
		wglSwapIntervalEXT(enabled);
	}

	const ContextDescription &ViewContextWGL::GetDescription() const
	{
		return m_Description;
	}

	HGLRC ViewContextWGL::CreateSharedContext(HDC hdc, HGLRC sharedContext, const ContextDescription &spec)
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

		// not required anymore
		/*if (spec.Samples >= 1)
		{
			iAttributes.push_back(WGL_SAMPLE_BUFFERS_ARB);
			iAttributes.push_back(GL_TRUE);
			iAttributes.push_back(WGL_SAMPLES_ARB);
			iAttributes.push_back(spec.Samples);
		}*/

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
		BOOL pixelFormatSet = SetPixelFormat(hdc, pixelFormat, &pfd);
		assert(pixelFormatSet && "Failed to set pixel format");

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
		NX_VALIDATE(wglShareLists(sharedContext, hglrc), "Failed to share contexts");

		NX_VALIDATE(hglrc, "Failed to create hglrc");

		return hglrc;
	}

	bool ViewContextWGL::Validate()
	{
		return m_HWND != nullptr && m_HDC != nullptr && m_HGLRC != nullptr;
	}

	const GladGLContext &ViewContextWGL::GetContext() const
	{
		return m_FunctionContext;
	}
}	 // namespace Nexus::GL