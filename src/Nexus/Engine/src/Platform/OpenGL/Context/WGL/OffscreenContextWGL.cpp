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

	std::expected<uint32_t, std::string> OffscreenContextWGL::CreateTexelBuffer(const Graphics::TexelBufferDescription &desc)
	{
		MakeCurrent();
		return m_FunctionContext.CreateTexelBuffer(desc);
	}

	void OffscreenContextWGL::DestroyTextureBuffer(uint32_t handle)
	{
		MakeCurrent();
		m_FunctionContext.DestroyTextureBuffer(handle);
	}

	void OffscreenContextWGL::BindTextureBuffer(uint32_t handle, uint32_t slot)
	{
		MakeCurrent();
		m_FunctionContext.BindTextureBuffer(handle, slot);
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

	uint32_t OffscreenContextWGL::CreateProgram()
	{
		MakeCurrent();
		return m_FunctionContext.CreateProgram();
	}

	void OffscreenContextWGL::AttachShaderModule(uint32_t program, uint32_t shader)
	{
		MakeCurrent();
		m_FunctionContext.AttachShaderModule(program, shader);
	}

	void OffscreenContextWGL::LinkProgram(uint32_t program)
	{
		MakeCurrent();
		m_FunctionContext.LinkProgram(program);
	}

	int OffscreenContextWGL::GetProgramiv(uint32_t program, GLenum parameter)
	{
		MakeCurrent();
		return m_FunctionContext.GetProgramiv(program, parameter);
	}

	void OffscreenContextWGL::GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
	{
		MakeCurrent();
		m_FunctionContext.GetProgramInfoLog(program, maxLength, length, infoLog);
	}

	void OffscreenContextWGL::DetachShader(uint32_t program, uint32_t shader)
	{
		MakeCurrent();
		m_FunctionContext.DetachShader(program, shader);
	}

	void OffscreenContextWGL::UseShader(uint32_t program)
	{
		MakeCurrent();
		m_FunctionContext.UseShader(program);
	}

	// pipeline state
	void OffscreenContextWGL::EnableCapability(GLenum capability, bool enable)
	{
		MakeCurrent();
		m_FunctionContext.EnableCapability(capability, enable);
	}

	void OffscreenContextWGL::SetStencilMask(uint32_t mask)
	{
		MakeCurrent();
		m_FunctionContext.SetStencilMask(mask);
	}

	void OffscreenContextWGL::SetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		MakeCurrent();
		m_FunctionContext.SetStencilOp(face, sfail, dpfail, dppass);
	}

	void OffscreenContextWGL::SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
	{
		MakeCurrent();
		m_FunctionContext.SetStencilFunc(face, func, ref, mask);
	}

	void OffscreenContextWGL::EnableDepthMask(bool enable)
	{
		MakeCurrent();
		m_FunctionContext.EnableDepthMask(enable);
	}

	bool OffscreenContextWGL::IsDepthBoundsSupported()
	{
		MakeCurrent();
		return m_FunctionContext.IsDepthBoundsSupported();
	}

	void OffscreenContextWGL::SetDepthBounds(float min, float max)
	{
		MakeCurrent();
		m_FunctionContext.SetDepthBounds(min, max);
	}

	void OffscreenContextWGL::SetDepthMask(bool enabled)
	{
		MakeCurrent();
		m_FunctionContext.SetDepthMask(enabled);
	}

	void OffscreenContextWGL::SetDepthFunction(GLenum func)
	{
		MakeCurrent();
		m_FunctionContext.SetDepthFunction(func);
	}

	void OffscreenContextWGL::SetFaceCulling(GLenum cullMode)
	{
		MakeCurrent();
		m_FunctionContext.SetFaceCulling(cullMode);
	}

	bool OffscreenContextWGL::IsDepthClampSupported()
	{
		MakeCurrent();
		return m_FunctionContext.IsDepthClampSupported();
	}

	void OffscreenContextWGL::SetPolygonMode(GLenum face, GLenum mode)
	{
		MakeCurrent();
		m_FunctionContext.SetPolygonMode(face, mode);
	}

	void OffscreenContextWGL::SetFrontFace(GLenum face)
	{
		MakeCurrent();
		m_FunctionContext.SetFrontFace(face);
	}

	bool OffscreenContextWGL::SupportsPerTargetColourMask()
	{
		MakeCurrent();
		return m_FunctionContext.SupportsPerTargetColourMask();
	}

	void OffscreenContextWGL::SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		MakeCurrent();
		m_FunctionContext.SetColourMask(red, green, blue, alpha);
	}

	void OffscreenContextWGL::SetColourMaski(uint32_t index, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		MakeCurrent();
		m_FunctionContext.SetColourMaski(index, red, green, blue, alpha);
	}

	bool OffscreenContextWGL::SupportsPerTargetBlendFunction()
	{
		MakeCurrent();
		return m_FunctionContext.SupportsPerTargetBlendFunction();
	}

	void OffscreenContextWGL::SetBlendFunction(GLenum sfactor, GLenum dfactor)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendFunction(sfactor, dfactor);
	}

	void OffscreenContextWGL::SetBlendFunctioni(uint32_t index, GLenum sfactor, GLenum dfactor)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendFunctioni(index, sfactor, dfactor);
	}

	void OffscreenContextWGL::SetBlendFunctionSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendFunctionSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
	}

	void OffscreenContextWGL::SetBlendFunctionSeparatei(uint32_t index, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendFunctionSeparatei(index, srcRGB, dstRGB, srcAlpha, dstAlpha);
	}

	void OffscreenContextWGL::SetBlendEquation(GLenum mode)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendEquation(mode);
	}

	void OffscreenContextWGL::SetBlendEquationi(uint32_t index, GLenum mode)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendEquationi(index, mode);
	}

	void OffscreenContextWGL::SetBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendEquationSeparate(modeRGB, modeAlpha);
	}

	void OffscreenContextWGL::SetBlendEquationSeparatei(uint32_t index, GLenum modeRGB, GLenum modeAlpha)
	{
		MakeCurrent();
		m_FunctionContext.SetBlendEquationSeparatei(index, modeRGB, modeAlpha);
	}

	// buffers
	uint32_t OffscreenContextWGL::CreateVertexArray()
	{
		MakeCurrent();
		return m_FunctionContext.CreateVertexArray();
	}

	void OffscreenContextWGL::DestroyVertexArray(uint32_t vao)
	{
		MakeCurrent();
		m_FunctionContext.DestroyVertexArray(vao);
	}

	void OffscreenContextWGL::BindBuffer(GLenum target, GLuint buffer)
	{
		MakeCurrent();
		m_FunctionContext.BindBuffer(target, buffer);
	}

	void OffscreenContextWGL::BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
	{
		MakeCurrent();
		m_FunctionContext.BindBufferRange(target, index, buffer, offset, size);
	}

	void OffscreenContextWGL::BindVertexArray(uint32_t vao)
	{
		MakeCurrent();
		m_FunctionContext.BindVertexArray(vao);
	}

	void OffscreenContextWGL::EnableVertexAttribArray(uint32_t vao, uint32_t index)
	{
		MakeCurrent();
		m_FunctionContext.EnableVertexAttribArray(vao, index);
	}

	void OffscreenContextWGL::DisableVertexAttribArray(uint32_t vao, uint32_t index)
	{
		MakeCurrent();
		m_FunctionContext.DisableVertexAttribArray(vao, index);
	}

	void OffscreenContextWGL::SetVertexAttribPointer(uint32_t  vao,
													 uint32_t  vbo,
													 GLuint	   index,
													 GLint	   size,
													 GLenum	   type,
													 GLboolean normalized,
													 GLsizei   stride,
													 uint32_t  offset)
	{
		MakeCurrent();
		m_FunctionContext.SetVertexAttribPointer(vao, vbo, index, size, type, normalized, stride, offset);
	}

	void OffscreenContextWGL::SetVertexAttribIPointer(uint32_t vao,
													  uint32_t vbo,
													  GLuint   index,
													  GLint	   size,
													  GLenum   type,
													  GLsizei  stride,
													  uint32_t offset)
	{
		MakeCurrent();
		m_FunctionContext.SetVertexAttribIPointer(vao, vbo, index, size, type, stride, offset);
	}

	void OffscreenContextWGL::SetVertexAttribLPointer(uint32_t vao,
													  uint32_t vbo,
													  GLuint   index,
													  GLint	   size,
													  GLenum   type,
													  GLsizei  stride,
													  uint32_t offset)
	{
		MakeCurrent();
		m_FunctionContext.SetVertexAttribLPointer(vao, vbo, index, size, type, stride, offset);
	}

	void OffscreenContextWGL::SetVertexAttribDivisor(uint32_t vao, GLuint index, GLuint divisor)
	{
		MakeCurrent();
		m_FunctionContext.SetVertexAttribDivisor(vao, index, divisor);
	}

	bool OffscreenContextWGL::AreStorageBuffersSupported()
	{
		MakeCurrent();
		return m_FunctionContext.AreStorageBuffersSupported();
	}

	void OffscreenContextWGL::ShaderStorageBlockBinding(uint32_t shader, GLuint storageBlockIndex, GLuint storageBlockBinding)
	{
		MakeCurrent();
		m_FunctionContext.ShaderStorageBlockBinding(shader, storageBlockIndex, storageBlockBinding);
	}

	int32_t OffscreenContextWGL::GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name)
	{
		MakeCurrent();
		return m_FunctionContext.GetProgramResourceIndex(shader, programInterface, name);
	}

	int32_t OffscreenContextWGL::GetUniformBlockIndex(uint32_t shader, const GLchar *name)
	{
		MakeCurrent();
		return m_FunctionContext.GetUniformBlockIndex(shader, name);
	}

	int32_t OffscreenContextWGL::GetUniformLocation(uint32_t shader, const GLchar *name)
	{
		MakeCurrent();
		return m_FunctionContext.GetUniformLocation(shader, name);
	}

	void OffscreenContextWGL::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
		MakeCurrent();
		m_FunctionContext.UniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
	}

	void OffscreenContextWGL::BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
	{
		MakeCurrent();
		m_FunctionContext.BindImageTexture(unit, texture, level, layered, layer, access, format);
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