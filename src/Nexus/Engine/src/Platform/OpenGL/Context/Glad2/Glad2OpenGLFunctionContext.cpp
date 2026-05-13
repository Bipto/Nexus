#include "Platform/OpenGL/Context/Glad2/Glad2OpenGLFunctionContext.hpp"

#include "Platform/OpenGL/GL.hpp"

#include "Platform/OpenGL/DeviceBufferOpenGL.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::GL
{
	OpenGLFunctionContext::OpenGLFunctionContext()
	{
	}

	OpenGLFunctionContext::~OpenGLFunctionContext()
	{
		gladLoaderUnloadGLContext(&m_Context);
	}

	bool OpenGLFunctionContext::Load()
	{
		int result = gladLoaderLoadGLContext(&m_Context);
		return result;
	}

	void OpenGLFunctionContext::ExecuteCommands(std::function<void(const GladGLContext &context)> function)
	{
		function(m_Context);
	}

	const GladGLContext &OpenGLFunctionContext::GetContext() const
	{
		return m_Context;
	}

	std::expected<uint32_t, std::string> OpenGLFunctionContext::CreateTexture(const Graphics::TextureDescription &desc)
	{
		return std::expected<uint32_t, std::string>();
	}

	std::expected<uint32_t, std::string> OpenGLFunctionContext::CreateTexelBuffer(const Graphics::TexelBufferDescription &desc)
	{
		GLenum								internalFormat = GL::GetSizedInternalFormat(desc.Format);
		const Graphics::DeviceBufferOpenGL *buffer		   = desc.Buffer.AsDerived<const Graphics::DeviceBufferOpenGL>();

		NX_VALIDATE(buffer, "Invalid buffer supplied when attempting to create texel buffer");

		uint32_t handle = 0;

		if (m_Context.CreateTextures != nullptr && m_Context.TextureBufferRange != nullptr)
		{
			m_Context.CreateTextures(GL_TEXTURE_BUFFER, 1, &handle);
			m_Context.TextureBufferRange(handle, internalFormat, buffer->GetHandle(), desc.Offset, desc.SizeInBytes);
		}
		else if (m_Context.TexBufferRangeEXT != nullptr)
		{
			m_Context.GenTextures(1, &handle);
			m_Context.BindTexture(GL_TEXTURE_BUFFER, handle);
			m_Context.TexBufferRange(GL_TEXTURE_BUFFER, internalFormat, buffer->GetHandle(), desc.Offset, desc.SizeInBytes);
		}
		else
		{
			return std::unexpected("Texel buffers are not supported");
		}

		return handle;
	}

	void OpenGLFunctionContext::DestroyTextureBuffer(uint32_t handle)
	{
		glCall(m_Context.DeleteTextures(1, &handle));
	}

	void OpenGLFunctionContext::BindTextureBuffer(uint32_t handle, uint32_t slot)
	{
		if (m_Context.BindTextureUnit != nullptr)
		{
			glCall(m_Context.BindTextureUnit(slot, handle));
		}
		else
		{
			glCall(m_Context.ActiveTexture(GL_TEXTURE0 + slot));
			glCall(m_Context.BindTexture(GL_TEXTURE_BUFFER, handle));
		}
	}

	std::expected<uint32_t, std::string> OpenGLFunctionContext::CreateSampler(const Graphics::SamplerDescription &desc)
	{
		uint32_t handle = 0;
		glCall(m_Context.GenSamplers(1, &handle));

		if (m_Context.KHR_debug)
		{
			// the sampler must have been bound at least once to name it
			glCall(m_Context.BindSampler(0, handle));
			glCall(m_Context.ObjectLabelKHR(GL_SAMPLER, handle, -1, desc.DebugName.c_str()));

			bool useMips = desc.MinimumLOD != 0 || desc.MaximumLOD != 0;

			GLenum min, max;
			GL::GetSamplerFilter(desc.SampleFilter, min, max, useMips);

			// texture sampling options
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, min));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, max));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_WRAP_S, GL::GetSamplerAddressMode(desc.AddressModeU)));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_WRAP_T, GL::GetSamplerAddressMode(desc.AddressModeV)));
			glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_WRAP_R, GL::GetSamplerAddressMode(desc.AddressModeW)));

			// texture anisotropy
			if (desc.SampleFilter == Graphics::SamplerFilter::Anisotropic)
			{
				glCall(m_Context.SamplerParameterf(handle, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, desc.MaximumAnisotropy));
			}

			const glm::vec4 color = Nexus::Utils::ColourFromBorderColor(desc.TextureBorderColor);

			// border colour
			GLfloat border[] = {color.r, color.g, color.b, color.a};
			glCall(m_Context.SamplerParameterfv(handle, GL_TEXTURE_BORDER_COLOR, border));

			// LOD
			glCall(m_Context.SamplerParameterf(handle, GL_TEXTURE_MIN_LOD, desc.MinimumLOD));
			glCall(m_Context.SamplerParameterf(handle, GL_TEXTURE_MAX_LOD, desc.MaximumLOD));

			if (m_Context.EXT_texture_lod_bias)
			{
				glCall(m_Context.SamplerParameterf(handle, GL_TEXTURE_LOD_BIAS_EXT, desc.LODBias));
			}

			// texture comparison
			if (desc.SamplerComparisonFunction != Graphics::ComparisonFunction::Never)
			{
				auto comparisonFunction = GL::GetComparisonFunction(desc.SamplerComparisonFunction);
				glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
				glCall(m_Context.SamplerParameteri(handle, GL_TEXTURE_COMPARE_FUNC, comparisonFunction));
			}
		}

		return handle;
	}

	void OpenGLFunctionContext::DestroySampler(uint32_t handle)
	{
		glCall(m_Context.DeleteSamplers(1, &handle));
	}

	void OpenGLFunctionContext::BindSampler(uint32_t handle, uint32_t slot)
	{
		glCall(m_Context.BindSampler(slot, handle));
	}

	std::expected<GLsync, std::string> OpenGLFunctionContext::CreateFence(const Graphics::FenceDescription &desc)
	{
		GLsync handle = m_Context.FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		// wait for the new fence to be signalled
		if (desc.Signalled)
		{
			GLenum result = WaitForFence(handle, 0);
			if (result == GL_WAIT_FAILED)
			{
				throw std::runtime_error("Failed to wait for fence");
			}
		}

		if (m_Context.KHR_debug)
		{
			m_Context.ObjectPtrLabelKHR(handle, -1, desc.DebugName.c_str());
		}

		return handle;
	}

	void OpenGLFunctionContext::DestroyFence(GLsync handle)
	{
		m_Context.DeleteSync(handle);
	}

	bool OpenGLFunctionContext::IsSignalled(GLsync handle)
	{
		GLint status = -1;
		m_Context.GetSynciv(handle, GL_SYNC_STATUS, sizeof(status), nullptr, &status);
		return status == GL_SIGNALED;
	}

	GLenum OpenGLFunctionContext::WaitForFence(GLsync handle, uint64_t timeout)
	{
		return m_Context.ClientWaitSync(handle, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
	}

	uint32_t OpenGLFunctionContext::CreateProgram()
	{
		return m_Context.CreateProgram();
	}

	void OpenGLFunctionContext::AttachShaderModule(uint32_t program, uint32_t shader)
	{
		glCall(m_Context.AttachShader(program, shader));
	}

	void OpenGLFunctionContext::LinkProgram(uint32_t program)
	{
		glCall(m_Context.LinkProgram(program));
	}

	int OpenGLFunctionContext::GetProgramiv(uint32_t program, GLenum parameter)
	{
		int returnValue = 0;
		glCall(m_Context.GetProgramiv(program, parameter, &returnValue));
		return returnValue;
	}

	void OpenGLFunctionContext::GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
	{
		glCall(m_Context.GetProgramInfoLog(program, maxLength, nullptr, infoLog));
	}

	void OpenGLFunctionContext::DetachShader(uint32_t program, uint32_t shader)
	{
		glCall(m_Context.DetachShader(program, shader));
	}

	void OpenGLFunctionContext::UseShader(uint32_t program)
	{
		glCall(m_Context.UseProgram(program));
	}

	// pipeline state
	void OpenGLFunctionContext::EnableCapability(GLenum capability, bool enable)
	{
		if (enable)
		{
			glCall(m_Context.Enable(capability));
		}
		else
		{
			glCall(m_Context.Disable(capability));
		}
	}

	void OpenGLFunctionContext::SetStencilMask(uint32_t mask)
	{
		glCall(m_Context.StencilMask(mask));
	}

	void OpenGLFunctionContext::SetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		glCall(m_Context.StencilOpSeparate(face, sfail, dpfail, dppass));
	}

	void OpenGLFunctionContext::SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
	{
		glCall(m_Context.StencilFuncSeparate(face, func, ref, mask));
	}

	void OpenGLFunctionContext::EnableDepthMask(bool enable)
	{
		glCall(m_Context.DepthMask(enable ? GL_TRUE : GL_FALSE));
	}

	bool OpenGLFunctionContext::IsDepthBoundsSupported()
	{
		return m_Context.DepthBoundsEXT != nullptr;
	}

	void OpenGLFunctionContext::SetDepthBounds(float min, float max)
	{
		glCall(m_Context.DepthBoundsEXT(min, max));
	}

	void OpenGLFunctionContext::SetDepthMask(bool enabled)
	{
		glCall(m_Context.DepthMask(enabled ? GL_TRUE : GL_FALSE));
	}

	void OpenGLFunctionContext::SetDepthFunction(GLenum func)
	{
		glCall(m_Context.DepthFunc(func));
	}

	void OpenGLFunctionContext::SetFaceCulling(GLenum cullMode)
	{
		glCall(m_Context.CullFace(cullMode));
	}

	bool OpenGLFunctionContext::IsDepthClampSupported()
	{
		return m_Context.EXT_depth_clamp == 1;
	}

	void OpenGLFunctionContext::SetPolygonMode(GLenum face, GLenum mode)
	{
		glCall(m_Context.PolygonMode(face, mode));
	}

	void OpenGLFunctionContext::SetFrontFace(GLenum face)
	{
		glCall(m_Context.FrontFace(face));
	}

	bool OpenGLFunctionContext::SupportsPerTargetColourMask()
	{
		return m_Context.ColorMaski != nullptr;
	}

	void OpenGLFunctionContext::SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		glCall(m_Context.ColorMask(red, green, blue, alpha));
	}

	void OpenGLFunctionContext::SetColourMaski(uint32_t index, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		glCall(m_Context.ColorMaski(index, red, green, blue, alpha));
	}

	bool OpenGLFunctionContext::SupportsPerTargetBlendFunction()
	{
		return m_Context.BlendFunci != nullptr;
	}

	void OpenGLFunctionContext::SetBlendFunction(GLenum sfactor, GLenum dfactor)
	{
		glCall(m_Context.BlendFunc(sfactor, dfactor));
	}

	void OpenGLFunctionContext::SetBlendFunctioni(uint32_t index, GLenum sfactor, GLenum dfactor)
	{
		glCall(m_Context.BlendFunci(index, sfactor, dfactor));
	}

	void OpenGLFunctionContext::SetBlendFunctionSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		glCall(m_Context.BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha));
	}

	void OpenGLFunctionContext::SetBlendFunctionSeparatei(uint32_t index, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		glCall(m_Context.BlendFuncSeparatei(index, srcRGB, dstRGB, srcAlpha, dstAlpha));
	}

	void OpenGLFunctionContext::SetBlendEquation(GLenum mode)
	{
		glCall(m_Context.BlendEquation(mode));
	}

	void OpenGLFunctionContext::SetBlendEquationi(uint32_t index, GLenum mode)
	{
		glCall(m_Context.BlendEquationi(index, mode));
	}

	void OpenGLFunctionContext::SetBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
	{
		glCall(m_Context.BlendEquationSeparate(modeRGB, modeAlpha));
	}

	void OpenGLFunctionContext::SetBlendEquationSeparatei(uint32_t index, GLenum modeRGB, GLenum modeAlpha)
	{
		glCall(m_Context.BlendEquationSeparatei(index, modeRGB, modeAlpha));
	}

	// buffers
	uint32_t OpenGLFunctionContext::CreateVertexArray()
	{
		uint32_t handle = 0;

		if (m_Context.CreateVertexArrays != nullptr)
		{
			glCall(m_Context.CreateVertexArrays(1, &handle));
		}
		else
		{
			glCall(m_Context.GenVertexArrays(1, &handle));
		}

		return handle;
	}

	void OpenGLFunctionContext::DestroyVertexArray(uint32_t vao)
	{
		glCall(m_Context.BindVertexArray(0));
		glCall(m_Context.DeleteVertexArrays(1, &vao));
	}

	void OpenGLFunctionContext::BindBuffer(GLenum target, GLuint buffer)
	{
		glCall(m_Context.BindBuffer(target, buffer));
	}

	void OpenGLFunctionContext::BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
	{
		glCall(m_Context.BindBufferRange(target, index, buffer, offset, size));
	}

	void OpenGLFunctionContext::BindVertexArray(uint32_t vao)
	{
		glCall(m_Context.BindVertexArray(vao));
	}

	void OpenGLFunctionContext::EnableVertexAttribArray(uint32_t vao, uint32_t index)
	{
		if (m_Context.EnableVertexArrayAttrib != nullptr)
		{
			glCall(m_Context.EnableVertexArrayAttrib(vao, index));
		}
		else if (m_Context.EnableVertexArrayAttribEXT != nullptr)
		{
			glCall(m_Context.EnableVertexArrayAttribEXT(vao, index));
		}
		else
		{
			glCall(m_Context.BindVertexArray(vao));
			glCall(m_Context.EnableVertexAttribArray(index));
		}
	}

	void OpenGLFunctionContext::DisableVertexAttribArray(uint32_t vao, uint32_t index)
	{
		if (m_Context.DisableVertexArrayAttrib != nullptr)
		{
			glCall(m_Context.DisableVertexArrayAttrib(vao, index));
		}
		else if (m_Context.DisableVertexArrayAttribEXT != nullptr)
		{
			glCall(m_Context.DisableVertexArrayAttribEXT(vao, index));
		}
		else
		{
			glCall(m_Context.BindVertexArray(vao));
			glCall(m_Context.EnableVertexAttribArray(index));
		}
	}

	void OpenGLFunctionContext::SetVertexAttribPointer(uint32_t	 vao,
													   uint32_t	 vbo,
													   GLuint	 index,
													   GLint	 size,
													   GLenum	 type,
													   GLboolean normalized,
													   GLsizei	 stride,
													   uint32_t	 offset)
	{
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void OpenGLFunctionContext::SetVertexAttribIPointer(uint32_t vao,
														uint32_t vbo,
														GLuint	 index,
														GLint	 size,
														GLenum	 type,
														GLsizei	 stride,
														uint32_t offset)
	{
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribIPointer(index, size, type, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void OpenGLFunctionContext::SetVertexAttribLPointer(uint32_t vao,
														uint32_t vbo,
														GLuint	 index,
														GLint	 size,
														GLenum	 type,
														GLsizei	 stride,
														uint32_t offset)
	{
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribLPointer(index, size, type, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void OpenGLFunctionContext::SetVertexAttribDivisor(uint32_t vao, GLuint index, GLuint divisor)
	{
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.VertexAttribDivisor(index, divisor));
	}

	bool OpenGLFunctionContext::AreStorageBuffersSupported()
	{
		return m_Context.ARB_shader_storage_buffer_object || (m_Context.VERSION_4_5 == 1 || m_Context.ES_VERSION_3_1);
	}

	int32_t OpenGLFunctionContext::GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name)
	{
		int32_t location = 0;
		glCall(location = m_Context.GetProgramResourceIndex(shader, programInterface, name));
		return location;
	}

	int32_t OpenGLFunctionContext::GetUniformBlockIndex(uint32_t shader, const GLchar *name)
	{
		int32_t location = 0;
		glCall(location = m_Context.GetUniformBlockIndex(shader, name));
		return location;
	}

	int32_t OpenGLFunctionContext::GetUniformLocation(uint32_t shader, const GLchar *name)
	{
		int32_t location = 0;
		glCall(location = m_Context.GetUniformLocation(shader, name));
		return location;
	}

	void OpenGLFunctionContext::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
		glCall(m_Context.UniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding));
	}

	void OpenGLFunctionContext::BindImageTexture(GLuint	   unit,
												 GLuint	   texture,
												 GLint	   level,
												 GLboolean layered,
												 GLint	   layer,
												 GLenum	   access,
												 GLenum	   format)
	{
		glCall(m_Context.BindImageTexture(unit, texture, level, layered, layer, access, format));
	}

}	 // namespace Nexus::GL
