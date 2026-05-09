#include "OffscreenContextWGL.hpp"
#include "PhysicalDeviceWGL.hpp"

#include "Platform/OpenGL/GL.hpp"

namespace Nexus::GL
{
	OffscreenContextWGL::OffscreenContextWGL(const ContextDescription &spec, Graphics::IPhysicalDevice *device)
	{
		Graphics::PhysicalDeviceWGL *deviceWGL = (Graphics::PhysicalDeviceWGL *)device;

		auto [pbuffer, hdc, hglrc] = CreatePBufferContext(deviceWGL->GetHDC(), spec);

		m_PBuffer = pbuffer;
		m_HDC	  = hdc;
		m_HGLRC	  = hglrc;

		wglMakeCurrent(m_HDC, m_HGLRC);
		m_FunctionContext.Load();
	}

	OffscreenContextWGL::~OffscreenContextWGL()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_HGLRC);
		wglDestroyPbufferARB(m_PBuffer);
	}

	bool OffscreenContextWGL::MakeCurrent()
	{
		HGLRC currentContext = wglGetCurrentContext();
		HDC	  currentHDC	 = wglGetCurrentDC();

		// the context is already current, so we can skip this step
		if (currentContext == m_HGLRC && currentHDC == m_HDC)
		{
			return true;
		}

		return wglMakeCurrent(m_HDC, m_HGLRC);
	}

	HGLRC OffscreenContextWGL::GetHGLRC()
	{
		return m_HGLRC;
	}

	std::expected<uint32_t, std::string> OffscreenContextWGL::CreateTexture(const Graphics::TextureDescription &desc)
	{
		MakeCurrent();
		return std::expected<uint32_t, std::string>();
	}

	std::expected<uint32_t, std::string> OffscreenContextWGL::CreateSampler(const Graphics::SamplerDescription &desc)
	{
		MakeCurrent();
		return m_FunctionContext.CreateSampler(desc);
	}

	void OffscreenContextWGL::DestroySampler(uint32_t handle)
	{
		MakeCurrent();
		m_FunctionContext.DestroySampler(handle);
	}

	void OffscreenContextWGL::BindSampler(uint32_t handle, uint32_t slot)
	{
		MakeCurrent();
		m_FunctionContext.BindSampler(handle, slot);
	}

	std::expected<GLsync, std::string> OffscreenContextWGL::CreateFence(const Graphics::FenceDescription &desc)
	{
		MakeCurrent();
		return m_FunctionContext.CreateFence(desc);
	}

	void OffscreenContextWGL::DestroyFence(GLsync handle)
	{
		MakeCurrent();
		m_FunctionContext.DestroyFence(handle);
	}

	bool OffscreenContextWGL::IsSignalled(GLsync handle)
	{
		MakeCurrent();
		return m_FunctionContext.IsSignalled(handle);
	}

	GLenum OffscreenContextWGL::WaitForFence(GLsync handle, uint64_t timeout)
	{
		MakeCurrent();
		return m_FunctionContext.WaitForFence(handle, timeout);
	}

	std::tuple<HPBUFFERARB, HDC, HGLRC> OffscreenContextWGL::CreatePBufferContext(HDC hdc, const ContextDescription &spec)
	{
		int	 pixelFormat;
		UINT numFormats;
		int	 attributes[] = {WGL_DRAW_TO_PBUFFER_ARB,
							 GL_TRUE,
							 WGL_SUPPORT_OPENGL_ARB,
							 GL_TRUE,
							 WGL_PIXEL_TYPE_ARB,
							 WGL_TYPE_RGBA_ARB,
							 WGL_ACCELERATION_ARB,
							 WGL_FULL_ACCELERATION_ARB,
							 WGL_COLOR_BITS_ARB,
							 24,
							 WGL_DEPTH_BITS_ARB,
							 16,
							 0};

		if (!wglChoosePixelFormatARB(hdc, attributes, NULL, 1, &pixelFormat, &numFormats))
		{
			std::cout << "Failed to choose pixel format" << std::endl;
		}

		int pbufferAttributes[] = {0};

		HPBUFFERARB pbuffer = wglCreatePbufferARB(hdc, pixelFormat, 1, 1, pbufferAttributes);
		NX_VALIDATE(pbuffer, "Failed to create PBuffer");

		HDC pbufferDC = wglGetPbufferDCARB(pbuffer);
		NX_VALIDATE(pbufferDC, "Failed to create DC for PBuffer");

		std::vector<int> contextAttributes;
		contextAttributes.push_back(WGL_CONTEXT_MAJOR_VERSION_ARB);
		contextAttributes.push_back(spec.VersionMajor);
		contextAttributes.push_back(WGL_CONTEXT_MINOR_VERSION_ARB);
		contextAttributes.push_back(spec.VersionMinor);
		contextAttributes.push_back(WGL_CONTEXT_PROFILE_MASK_ARB);

		if (spec.UseCoreProfile)
		{
			contextAttributes.push_back(WGL_CONTEXT_CORE_PROFILE_BIT_ARB);
		}
		else
		{
			contextAttributes.push_back(WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB);
		}

		if (spec.Debug)
		{
			contextAttributes.push_back(WGL_CONTEXT_FLAGS_ARB);
			contextAttributes.push_back(WGL_CONTEXT_DEBUG_BIT_ARB);
		}
		else
		{
			contextAttributes.push_back(WGL_CONTEXT_OPENGL_NO_ERROR_ARB);
			contextAttributes.push_back(TRUE);
		}

		contextAttributes.push_back(0);

		HGLRC pbufferContext = wglCreateContextAttribsARB(pbufferDC, NULL, contextAttributes.data());
		NX_VALIDATE(pbufferContext, "Failed to create OpenGL context");

		return {pbuffer, pbufferDC, pbufferContext};
	}

	bool OffscreenContextWGL::Validate()
	{
		return m_HGLRC != nullptr && m_PBuffer != nullptr && m_HDC != nullptr;
	}

	const GladGLContext &OffscreenContextWGL::GetContext() const
	{
		return m_FunctionContext.GetContext();
	}
}	 // namespace Nexus::GL