#include "Platform/OpenGL/Context/Glad2/OffscreenContextGlad2.hpp"

#include "Platform/OpenGL/GL.hpp"

#include "Platform/OpenGL/DeviceBufferOpenGL.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::GL
{
	OffscreenContextGlad2::OffscreenContextGlad2()
	{
	}

	OffscreenContextGlad2::~OffscreenContextGlad2()
	{
		gladLoaderUnloadGLContext(&m_Context);
	}

	bool OffscreenContextGlad2::Load()
	{
		int result = gladLoaderLoadGLContext(&m_Context);
		return result;
	}

	void OffscreenContextGlad2::ExecuteCommands(std::function<void(const GladGLContext &context)> function)
	{
		function(m_Context);
	}

	const GladGLContext &OffscreenContextGlad2::GetContext() const
	{
		return m_Context;
	}

	std::expected<uint32_t, std::string> OffscreenContextGlad2::CreateTexture(const Graphics::TextureDescription &desc)
	{
		MakeCurrent();
		return std::expected<uint32_t, std::string>();
	}

	std::expected<uint32_t, std::string> OffscreenContextGlad2::CreateTexelBuffer(const Graphics::TexelBufferDescription &desc)
	{
		MakeCurrent();

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

	void OffscreenContextGlad2::DestroyTextureBuffer(uint32_t handle)
	{
		MakeCurrent();
		glCall(m_Context.DeleteTextures(1, &handle));
	}

	void OffscreenContextGlad2::BindTextureBuffer(uint32_t handle, uint32_t slot)
	{
		MakeCurrent();

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

	std::expected<uint32_t, std::string> OffscreenContextGlad2::CreateSampler(const Graphics::SamplerDescription &desc)
	{
		MakeCurrent();

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

	void OffscreenContextGlad2::DestroySampler(uint32_t handle)
	{
		MakeCurrent();
		glCall(m_Context.DeleteSamplers(1, &handle));
	}

	void OffscreenContextGlad2::BindSampler(uint32_t handle, uint32_t slot)
	{
		MakeCurrent();
		glCall(m_Context.BindSampler(slot, handle));
	}

	std::expected<GLsync, std::string> OffscreenContextGlad2::CreateFence(const Graphics::FenceDescription &desc)
	{
		MakeCurrent();
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

	void OffscreenContextGlad2::DestroyFence(GLsync handle)
	{
		MakeCurrent();
		m_Context.DeleteSync(handle);
	}

	bool OffscreenContextGlad2::IsSignalled(GLsync handle)
	{
		MakeCurrent();

		GLint status = -1;
		m_Context.GetSynciv(handle, GL_SYNC_STATUS, sizeof(status), nullptr, &status);
		return status == GL_SIGNALED;
	}

	GLenum OffscreenContextGlad2::WaitForFence(GLsync handle, uint64_t timeout)
	{
		MakeCurrent();
		return m_Context.ClientWaitSync(handle, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
	}

	uint32_t OffscreenContextGlad2::CreateProgram()
	{
		MakeCurrent();
		return m_Context.CreateProgram();
	}

	void OffscreenContextGlad2::AttachShaderModule(uint32_t program, uint32_t shader)
	{
		MakeCurrent();
		glCall(m_Context.AttachShader(program, shader));
	}

	void OffscreenContextGlad2::LinkProgram(uint32_t program)
	{
		MakeCurrent();
		glCall(m_Context.LinkProgram(program));
	}

	int OffscreenContextGlad2::GetProgramiv(uint32_t program, GLenum parameter)
	{
		MakeCurrent();
		int returnValue = 0;
		glCall(m_Context.GetProgramiv(program, parameter, &returnValue));
		return returnValue;
	}

	void OffscreenContextGlad2::GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
	{
		MakeCurrent();
		glCall(m_Context.GetProgramInfoLog(program, maxLength, nullptr, infoLog));
	}

	void OffscreenContextGlad2::DetachShader(uint32_t program, uint32_t shader)
	{
		MakeCurrent();
		glCall(m_Context.DetachShader(program, shader));
	}

	void OffscreenContextGlad2::UseShader(uint32_t program)
	{
		MakeCurrent();
		glCall(m_Context.UseProgram(program));
	}

	// pipeline state
	void OffscreenContextGlad2::EnableCapability(GLenum capability, bool enable)
	{
		MakeCurrent();

		if (enable)
		{
			glCall(m_Context.Enable(capability));
		}
		else
		{
			glCall(m_Context.Disable(capability));
		}
	}

	void OffscreenContextGlad2::SetStencilMask(uint32_t mask)
	{
		MakeCurrent();
		glCall(m_Context.StencilMask(mask));
	}

	void OffscreenContextGlad2::SetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		MakeCurrent();
		glCall(m_Context.StencilOpSeparate(face, sfail, dpfail, dppass));
	}

	void OffscreenContextGlad2::SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
	{
		MakeCurrent();
		glCall(m_Context.StencilFuncSeparate(face, func, ref, mask));
	}

	void OffscreenContextGlad2::EnableDepthMask(bool enable)
	{
		MakeCurrent();
		glCall(m_Context.DepthMask(enable ? GL_TRUE : GL_FALSE));
	}

	bool OffscreenContextGlad2::IsDepthBoundsSupported()
	{
		return m_Context.DepthBoundsEXT != nullptr;
	}

	void OffscreenContextGlad2::SetDepthBounds(float min, float max)
	{
		MakeCurrent();
		glCall(m_Context.DepthBoundsEXT(min, max));
	}

	void OffscreenContextGlad2::SetDepthMask(bool enabled)
	{
		MakeCurrent();
		glCall(m_Context.DepthMask(enabled ? GL_TRUE : GL_FALSE));
	}

	void OffscreenContextGlad2::SetDepthFunction(GLenum func)
	{
		MakeCurrent();
		glCall(m_Context.DepthFunc(func));
	}

	void OffscreenContextGlad2::SetFaceCulling(GLenum cullMode)
	{
		MakeCurrent();
		glCall(m_Context.CullFace(cullMode));
	}

	bool OffscreenContextGlad2::IsDepthClampSupported()
	{
		return m_Context.EXT_depth_clamp == 1;
	}

	void OffscreenContextGlad2::SetPolygonMode(GLenum face, GLenum mode)
	{
		MakeCurrent();
		glCall(m_Context.PolygonMode(face, mode));
	}

	void OffscreenContextGlad2::SetFrontFace(GLenum face)
	{
		MakeCurrent();
		glCall(m_Context.FrontFace(face));
	}

	bool OffscreenContextGlad2::SupportsPerTargetColourMask()
	{
		return m_Context.ColorMaski != nullptr;
	}

	void OffscreenContextGlad2::SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		MakeCurrent();
		glCall(m_Context.ColorMask(red, green, blue, alpha));
	}

	void OffscreenContextGlad2::SetColourMaski(uint32_t index, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		MakeCurrent();
		glCall(m_Context.ColorMaski(index, red, green, blue, alpha));
	}

	bool OffscreenContextGlad2::SupportsPerTargetBlendFunction()
	{
		return m_Context.BlendFunci != nullptr;
	}

	void OffscreenContextGlad2::SetBlendFunction(GLenum sfactor, GLenum dfactor)
	{
		MakeCurrent();
		glCall(m_Context.BlendFunc(sfactor, dfactor));
	}

	void OffscreenContextGlad2::SetBlendFunctioni(uint32_t index, GLenum sfactor, GLenum dfactor)
	{
		MakeCurrent();
		glCall(m_Context.BlendFunci(index, sfactor, dfactor));
	}

	void OffscreenContextGlad2::SetBlendFunctionSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha));
	}

	void OffscreenContextGlad2::SetBlendFunctionSeparatei(uint32_t index, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendFuncSeparatei(index, srcRGB, dstRGB, srcAlpha, dstAlpha));
	}

	void OffscreenContextGlad2::SetBlendEquation(GLenum mode)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquation(mode));
	}

	void OffscreenContextGlad2::SetBlendEquationi(uint32_t index, GLenum mode)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquationi(index, mode));
	}

	void OffscreenContextGlad2::SetBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquationSeparate(modeRGB, modeAlpha));
	}

	void OffscreenContextGlad2::SetBlendEquationSeparatei(uint32_t index, GLenum modeRGB, GLenum modeAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquationSeparatei(index, modeRGB, modeAlpha));
	}

	void OffscreenContextGlad2::BlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendColor(red, green, blue, alpha));
	}

	bool OffscreenContextGlad2::SupportsTextureBarriers()
	{
		return m_Context.TextureBarrier != nullptr || m_Context.TextureBarrierNV != nullptr;
	}

	void OffscreenContextGlad2::TextureBarrier()
	{
		if (m_Context.TextureBarrier)
		{
			glCall(m_Context.TextureBarrier());
		}
		else if (m_Context.TextureBarrierNV)
		{
			glCall(m_Context.TextureBarrierNV());
		}
	}

	bool OffscreenContextGlad2::SupportsMemoryBarriers()
	{
		return m_Context.MemoryBarrierEXT != nullptr;
	}

	void OffscreenContextGlad2::MemoryBarrierEXT(GLbitfield barriers)
	{
		if (m_Context.MemoryBarrierEXT)
		{
			glCall(m_Context.MemoryBarrierEXT(barriers));
		}
	}

	void OffscreenContextGlad2::Finish()
	{
		glCall(m_Context.Finish());
	}

	// buffers
	uint32_t OffscreenContextGlad2::CreateVertexArray()
	{
		MakeCurrent();
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

	void OffscreenContextGlad2::DestroyVertexArray(uint32_t vao)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(0));
		glCall(m_Context.DeleteVertexArrays(1, &vao));
	}

	void OffscreenContextGlad2::BindBuffer(GLenum target, GLuint buffer)
	{
		MakeCurrent();
		glCall(m_Context.BindBuffer(target, buffer));
	}

	void OffscreenContextGlad2::BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
	{
		MakeCurrent();
		glCall(m_Context.BindBufferRange(target, index, buffer, offset, size));
	}

	void OffscreenContextGlad2::BindVertexArray(uint32_t vao)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
	}

	void OffscreenContextGlad2::EnableVertexAttribArray(uint32_t vao, uint32_t index)
	{
		MakeCurrent();

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

	void OffscreenContextGlad2::DisableVertexAttribArray(uint32_t vao, uint32_t index)
	{
		MakeCurrent();

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

	void OffscreenContextGlad2::SetVertexAttribPointer(uint32_t	 vao,
													   uint32_t	 vbo,
													   GLuint	 index,
													   GLint	 size,
													   GLenum	 type,
													   GLboolean normalized,
													   GLsizei	 stride,
													   uint32_t	 offset)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void OffscreenContextGlad2::SetVertexAttribIPointer(uint32_t vao,
														uint32_t vbo,
														GLuint	 index,
														GLint	 size,
														GLenum	 type,
														GLsizei	 stride,
														uint32_t offset)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribIPointer(index, size, type, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void OffscreenContextGlad2::SetVertexAttribLPointer(uint32_t vao,
														uint32_t vbo,
														GLuint	 index,
														GLint	 size,
														GLenum	 type,
														GLsizei	 stride,
														uint32_t offset)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribLPointer(index, size, type, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void OffscreenContextGlad2::SetVertexAttribDivisor(uint32_t vao, GLuint index, GLuint divisor)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.VertexAttribDivisor(index, divisor));
	}

	bool OffscreenContextGlad2::AreStorageBuffersSupported()
	{
		return m_Context.ARB_shader_storage_buffer_object || (m_Context.VERSION_4_5 == 1 || m_Context.ES_VERSION_3_1);
	}

	void OffscreenContextGlad2::ShaderStorageBlockBinding(uint32_t shader, GLuint storageBlockIndex, GLuint storageBlockBinding)
	{
		MakeCurrent();
		glCall(m_Context.ShaderStorageBlockBinding(shader, storageBlockIndex, storageBlockBinding));
	}

	int32_t OffscreenContextGlad2::GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name)
	{
		MakeCurrent();
		int32_t location = 0;
		glCall(location = m_Context.GetProgramResourceIndex(shader, programInterface, name));
		return location;
	}

	int32_t OffscreenContextGlad2::GetUniformBlockIndex(uint32_t shader, const GLchar *name)
	{
		MakeCurrent();
		int32_t location = 0;
		glCall(location = m_Context.GetUniformBlockIndex(shader, name));
		return location;
	}

	int32_t OffscreenContextGlad2::GetUniformLocation(uint32_t shader, const GLchar *name)
	{
		MakeCurrent();
		int32_t location = 0;
		glCall(location = m_Context.GetUniformLocation(shader, name));
		return location;
	}

	void OffscreenContextGlad2::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
		MakeCurrent();
		glCall(m_Context.UniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding));
	}

	void OffscreenContextGlad2::BindImageTexture(GLuint	   unit,
												 GLuint	   texture,
												 GLint	   level,
												 GLboolean layered,
												 GLint	   layer,
												 GLenum	   access,
												 GLenum	   format)
	{
		MakeCurrent();
		glCall(m_Context.BindImageTexture(unit, texture, level, layered, layer, access, format));
	}

	void OffscreenContextGlad2::DrawArrays(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
	{
		MakeCurrent();
		if (primcount == 1)
		{
			glCall(m_Context.DrawArrays(mode, first, count));
		}
		else
		{
			glCall(m_Context.DrawArraysInstanced(mode, first, count, primcount));
		}
	}

	void OffscreenContextGlad2::DrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount)
	{
		MakeCurrent();
		if (primcount == 1)
		{
			glCall(m_Context.DrawElements(mode, count, type, indices));
		}
		else
		{
			glCall(m_Context.DrawElementsInstanced(mode, count, type, indices, primcount));
		}
	}

	void OffscreenContextGlad2::MultiDrawArraysIndirect(GLenum mode, const void *indirect, GLsizei drawCount, GLsizei stride)
	{
		MakeCurrent();
		if (m_Context.MultiDrawArraysIndirect)
		{
			glCall(m_Context.MultiDrawArraysIndirect(mode, indirect, drawCount, stride));
		}
		else
		{
			size_t offset = reinterpret_cast<size_t>(indirect);
			for (size_t i = 0; i < drawCount; i++)
			{
				glCall(m_Context.DrawArraysIndirect(mode, reinterpret_cast<const void *>(offset)));
				offset += stride;
			}
		}
	}

	void OffscreenContextGlad2::MultiDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride)
	{
		MakeCurrent();
		if (m_Context.MultiDrawElementsIndirect)
		{
			glCall(m_Context.MultiDrawElementsIndirect(mode, type, indirect, drawcount, stride));
		}
		else
		{
			size_t offset = reinterpret_cast<size_t>(indirect);
			for (size_t i = 0; i < drawcount; i++)
			{
				glCall(m_Context.DrawElementsIndirect(mode, type, indirect));
				offset += stride;
			}
		}
	}

	void OffscreenContextGlad2::DispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
	{
		MakeCurrent();
		glCall(m_Context.DispatchCompute(num_groups_x, num_groups_y, num_groups_z));
	}

	void OffscreenContextGlad2::DispatchComputeIndirect(GLintptr indirect)
	{
		MakeCurrent();
		glCall(m_Context.DispatchComputeIndirect(indirect));
	}

	void OffscreenContextGlad2::DrawMeshTasksEXT(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
	{
		MakeCurrent();
		if (m_Context.DrawMeshTasksEXT)
		{
			glCall(m_Context.DrawMeshTasksEXT(num_groups_x, num_groups_y, num_groups_z));
		}
	}

	void OffscreenContextGlad2::DrawMeshTasksIndirectEXT(GLintptr indirect, GLintptr drawCount, GLsizei stride)
	{
		MakeCurrent();
		if (m_Context.MultiDrawMeshTasksIndirectEXT)
		{
			glCall(m_Context.MultiDrawMeshTasksIndirectEXT((GLintptr)indirect, drawCount, stride));
		}
		else if (m_Context.DrawMeshTasksIndirectEXT)
		{
			size_t indirectOffset = indirect;
			for (uint32_t i = 0; i < drawCount; i++) { glCall(m_Context.DrawMeshTasksIndirectEXT((GLintptr)indirectOffset)); }
			indirectOffset += stride;
		}
	}

	void OffscreenContextGlad2::ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
	{
		MakeCurrent();
		glCall(m_Context.ClearBufferfv(buffer, drawbuffer, value));
	}

	void OffscreenContextGlad2::ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
	{
		MakeCurrent();
		glCall(m_Context.ClearBufferfi(buffer, drawbuffer, depth, stencil));
	}

	void OffscreenContextGlad2::GetIntegerv(GLenum pname, GLint *data)
	{
		MakeCurrent();
		glCall(m_Context.GetIntegerv(pname, data));
	}

	void OffscreenContextGlad2::Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		MakeCurrent();
		glCall(m_Context.Scissor(x, y, width, height));
	}

	void OffscreenContextGlad2::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		MakeCurrent();
		glCall(m_Context.Viewport(x, y, width, height));
	}

	void OffscreenContextGlad2::DepthRangef(GLfloat nearVal, GLfloat farVal)
	{
		MakeCurrent();
		glCall(m_Context.DepthRangef(nearVal, farVal));
	}

	bool OffscreenContextGlad2::IsComputeSupported()
	{
		return m_Context.DispatchCompute != nullptr;
	}

	bool OffscreenContextGlad2::IsIndirectRenderingSupported()
	{
		return m_Context.DrawArraysIndirect != nullptr && m_Context.DrawElementsIndirect != nullptr;
	}

	bool OffscreenContextGlad2::IsMeshTaskSupported()
	{
		return m_Context.DrawMeshTasksEXT != nullptr && m_Context.DrawMeshTasksIndirectEXT != nullptr;
	}

}	 // namespace Nexus::GL
