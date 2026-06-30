#include "IGLContext.hpp"

#include "Nexus-Core/Utils/Utils.hpp"
#include "Platform/OpenGL/DeviceBufferOpenGL.hpp"
#include "Platform/OpenGL/GL.hpp"

namespace Nexus::GL
{
	IGLContext::~IGLContext()
	{ gladLoaderUnloadGLContext(&m_Context); }

	bool IGLContext::LoadFunctions()
	{ return gladLoaderLoadGLContext(&m_Context); }

	std::expected<uint32_t, std::string> IGLContext::CreateTexture(const Graphics::TextureDescription &desc)
	{
		MakeCurrent();
		return std::expected<uint32_t, std::string>();
	}

	bool IGLContext::IsTextureTypeSupported(Graphics::TextureType type, uint32_t arrayLayers)
	{
		switch (type)
		{
			case Graphics::TextureType::Texture1D:
				if (arrayLayers > 1)
				{
					return m_Context.VERSION_3_0;
				}
				else
				{
					return m_Context.VERSION_1_0;
				}
			case Graphics::TextureType::Texture2D:
			{
				if (arrayLayers > 1)
				{
					return m_Context.VERSION_3_0 || m_Context.ES_VERSION_3_0;
				}
				else
				{
					return m_Context.VERSION_1_0 || m_Context.ES_VERSION_2_0;
				}
			}
			case Graphics::TextureType::Texture3D:
			{
				if (arrayLayers > 1)
				{
					return false;
				}
				else
				{
					return m_Context.VERSION_1_2 || m_Context.ES_VERSION_3_0;
				}
			}
			case Graphics::TextureType::TextureCube:
			{
				if (arrayLayers > 1)
				{
					return m_Context.VERSION_4_0 || m_Context.ES_VERSION_3_2;
				}
				else
				{
					return m_Context.VERSION_1_3 || m_Context.ES_VERSION_2_0;
				}
			}
			default: return false;
		}
	}

	void IGLContext::CompressedTexSubImage1D(GLuint		   texture,
											 GLint		   level,
											 GLint		   xoffset,
											 GLsizei	   width,
											 GLenum		   format,
											 GLsizei	   imageSize,
											 const GLvoid *data)
	{
		MakeCurrent();

		if (m_Context.CompressedTextureSubImage1D)
		{
			m_Context.CompressedTextureSubImage1D(texture, level, xoffset, width, format, imageSize, data);
		}
		else
		{
			m_Context.BindTexture(GL_TEXTURE_1D, texture);
			m_Context.CompressedTexSubImage1D(GL_TEXTURE_1D, level, xoffset, width, format, imageSize, data);
		}
	}

	void IGLContext::TexSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels)
	{
		MakeCurrent();

		if (m_Context.TextureSubImage1D)
		{
			m_Context.TextureSubImage1D(texture, level, xoffset, width, format, type, pixels);
		}
		else
		{
			m_Context.BindTexture(GL_TEXTURE_1D, texture);
			m_Context.TexSubImage1D(texture, level, xoffset, width, format, type, pixels);
		}
	}

	void IGLContext::CompressedTexSubImage2D(GLuint		 texture,
											 GLint		 level,
											 GLint		 xoffset,
											 GLint		 yoffset,
											 GLsizei	 width,
											 GLsizei	 height,
											 GLenum		 format,
											 GLsizei	 imageSize,
											 const void *data)
	{
		MakeCurrent();

		if (m_Context.CompressedTextureSubImage2D)
		{
			m_Context.CompressedTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, imageSize, data);
		}
		else
		{
			m_Context.BindTexture(GL_TEXTURE_2D, texture);
			m_Context.CompressedTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, imageSize, data);
		}
	}

	void IGLContext::TexSubImage2D(GLuint		 texture,
								   GLint		 level,
								   GLint		 xoffset,
								   GLint		 yoffset,
								   GLsizei		 width,
								   GLsizei		 height,
								   GLenum		 format,
								   GLenum		 type,
								   const GLvoid *pixels)
	{
		MakeCurrent();

		if (m_Context.TextureSubImage2D)
		{
			m_Context.TextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels);
		}
		else
		{
			m_Context.BindTexture(GL_TEXTURE_2D, texture);
			m_Context.TexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, pixels);
		}
	}

	void IGLContext::CompressedTexSubImage3D(GLuint		 texture,
											 GLint		 level,
											 GLint		 xoffset,
											 GLint		 yoffset,
											 GLint		 zoffset,
											 GLsizei	 width,
											 GLsizei	 height,
											 GLsizei	 depth,
											 GLenum		 format,
											 GLsizei	 imageSize,
											 const void *data)
	{
		MakeCurrent();

		if (m_Context.CompressedTextureSubImage3D)
		{
			m_Context.CompressedTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
		}
		else
		{
			m_Context.BindTexture(GL_TEXTURE_3D, texture);
			m_Context.CompressedTexSubImage3D(GL_TEXTURE_3D, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
		}
	}

	void IGLContext::TexSubImage3D(GLuint	   texture,
								   GLint	   level,
								   GLint	   xoffset,
								   GLint	   yoffset,
								   GLint	   zoffset,
								   GLsizei	   width,
								   GLsizei	   height,
								   GLsizei	   depth,
								   GLenum	   format,
								   GLenum	   type,
								   const void *pixels)
	{
		MakeCurrent();

		if (m_Context.TextureSubImage3D)
		{
			m_Context.TextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
		}
		else
		{
			m_Context.BindTexture(GL_TEXTURE_3D, texture);
			m_Context.TexSubImage3D(GL_TEXTURE_3D, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
		}
	}

	std::expected<uint32_t, std::string> IGLContext::CreateTexelBuffer(const Graphics::TexelBufferDescription &desc)
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

	void IGLContext::DestroyTextureBuffer(uint32_t handle)
	{
		MakeCurrent();
		glCall(m_Context.DeleteTextures(1, &handle));
	}

	void IGLContext::BindTextureBuffer(uint32_t handle, uint32_t slot)
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

	std::expected<void, std::string> IGLContext::CreateBuffer(GLuint			&buffer,
															  GLenum			 target,
															  GLsizeiptr		 size,
															  const GLvoid		*data,
															  GLbitfield		 mapFlags,
															  GLenum			 bufferUsage,
															  const std::string &debugName,
															  bool				&supportsPersistentMapping)
	{
		// try to use persistent mapping if the functionality is available
		if (m_Context.ARB_buffer_storage || m_Context.VERSION_4_4)
		{
			// use DSA if available
			if (m_Context.ARB_direct_state_access || m_Context.VERSION_4_5)
			{
				glCall(m_Context.CreateBuffers(1, &buffer));
				glCall(m_Context.NamedBufferStorage(buffer, size, data, mapFlags));
			}
			// or fall back to buffer storage with binding
			else
			{
				glCall(m_Context.GenBuffers(1, &buffer));
				glCall(m_Context.BindBuffer(GL_COPY_READ_BUFFER, buffer));
				glCall(m_Context.BufferStorage(GL_COPY_READ_BUFFER, size, data, mapFlags));
			}

			// tell the user that persistent mapping is available
			supportsPersistentMapping = true;
		}
		// fall back to legacy BufferData and binding
		else
		{
			glCall(m_Context.GenBuffers(1, &buffer));
			glCall(m_Context.BindBuffer(GL_COPY_READ_BUFFER, buffer));
			glCall(m_Context.BufferData(GL_COPY_READ_BUFFER, size, nullptr, bufferUsage));

			// tell the user that persistent mapping is NOT available
			supportsPersistentMapping = false;
		}

		// assign a debug name if available
		if (m_Context.KHR_debug)
		{
			glCall(m_Context.ObjectLabelKHR(GL_BUFFER, buffer, debugName.size(), debugName.c_str()));
		}

		return std::expected<void, std::string> {};
	}

	void IGLContext::DeleteBuffers(GLsizei n, const GLuint *buffers)
	{ glCall(m_Context.DeleteBuffers(n, buffers)); }

	void IGLContext::CopyBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizei size)
	{
		// use DSA if it is available
		if (m_Context.ARB_direct_state_access || m_Context.EXT_direct_state_access)
		{
			// execute copy operation
			glCall(m_Context.CopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size));
		}
		// we need to use legacy binding
		else
		{
			// bind the target buffers
			glCall(m_Context.BindBuffer(GL_COPY_READ_BUFFER, readBuffer));
			glCall(m_Context.BindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer));

			// execute the copy operation
			glCall(m_Context.CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size));

			// unbind the target buffers
			glCall(m_Context.BindBuffer(GL_COPY_READ_BUFFER, 0));
			glCall(m_Context.BindBuffer(GL_COPY_WRITE_BUFFER, 0));
		}
	}

	void IGLContext::BufferSubData(GLuint buffer, GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
	{
		if (m_Context.NamedBufferSubData)
		{
			glCall(m_Context.NamedBufferSubData(buffer, offset, size, data));
		}
		else
		{
			glCall(m_Context.BindBuffer(target, buffer));
			glCall(m_Context.BufferSubData(target, offset, size, data));
		}
	}

	void IGLContext::GetBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid *data)
	{
		if (m_Context.GetNamedBufferSubData)
		{
			glCall(m_Context.GetNamedBufferSubData(buffer, offset, size, data));
		}
		else if (m_Context.GetBufferSubData)
		{
			glCall(m_Context.BindBuffer(GL_COPY_READ_BUFFER, buffer));
			glCall(m_Context.GetBufferSubData(GL_COPY_READ_BUFFER, offset, size, data));
		}
		else
		{
			glCall(m_Context.BindBuffer(GL_COPY_READ_BUFFER, buffer));

			void *mappedData = m_Context.MapBufferRange(GL_COPY_READ_BUFFER, offset, size, GL_MAP_READ_BIT);
			if (mappedData)
			{
				memcpy(data, mappedData, size);
			}

			m_Context.UnmapBuffer(GL_COPY_READ_BUFFER);
		}
	}

	void *IGLContext::MapBufferRange(GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access)
	{
		if (m_Context.MapNamedBufferRange)
		{
			return m_Context.MapNamedBufferRange(buffer, offset, length, access);
		}
		else
		{
			glCall(m_Context.BindBuffer(GL_COPY_READ_BUFFER, buffer));
			return m_Context.MapBufferRange(GL_COPY_READ_BUFFER, offset, length, access);
		}
	}

	void IGLContext::FramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{ glCall(m_Context.FramebufferTexture1D(target, attachment, textarget, texture, level)); }

	void IGLContext::FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	{ glCall(m_Context.FramebufferTexture2D(target, attachment, textarget, texture, level)); }

	void IGLContext::FramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer)
	{ glCall(m_Context.FramebufferTexture3D(target, attachment, textarget, texture, level, layer)); }

	void IGLContext::FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
	{ glCall(m_Context.FramebufferTextureLayer(target, attachment, texture, level, layer)); }

	void IGLContext::FramebufferTextureMultiviewOVR(GLenum	target,
													GLenum	attachment,
													GLuint	texture,
													GLint	level,
													GLint	baseViewIndex,
													GLsizei numViews)
	{ glCall(m_Context.FramebufferTextureMultiviewOVR(target, attachment, texture, level, baseViewIndex, numViews)); }

	std::expected<uint32_t, std::string> IGLContext::CreateSampler(const Graphics::SamplerDescription &desc)
	{
		MakeCurrent();

		uint32_t handle = 0;
		glCall(m_Context.GenSamplers(1, &handle));

		if (m_Context.KHR_debug)
		{
			// the sampler must have been bound at least once to name it
			glCall(m_Context.BindSampler(0, handle));
			glCall(m_Context.ObjectLabelKHR(GL_SAMPLER, handle, -1, desc.DebugName.c_str()));
		}

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

		const glm::vec4 color = Nexus::Utils::ColourFromBorderColour(desc.TextureBorderColor);

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

		return handle;
	}

	void IGLContext::DestroySampler(uint32_t handle)
	{
		MakeCurrent();
		glCall(m_Context.DeleteSamplers(1, &handle));
	}

	void IGLContext::BindSampler(uint32_t handle, uint32_t slot)
	{
		MakeCurrent();
		glCall(m_Context.BindSampler(slot, handle));
	}

	std::expected<GLsync, std::string> IGLContext::CreateFence(const Graphics::FenceDescription &desc)
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

	void IGLContext::DestroyFence(GLsync handle)
	{
		MakeCurrent();
		m_Context.DeleteSync(handle);
	}

	bool IGLContext::IsSignalled(GLsync handle)
	{
		MakeCurrent();

		GLint status = -1;
		m_Context.GetSynciv(handle, GL_SYNC_STATUS, sizeof(status), nullptr, &status);
		return status == GL_SIGNALED;
	}

	GLenum IGLContext::WaitForFence(GLsync handle, uint64_t timeout)
	{
		MakeCurrent();
		return m_Context.ClientWaitSync(handle, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
	}

	uint32_t IGLContext::CreateProgram()
	{
		MakeCurrent();
		return m_Context.CreateProgram();
	}

	void IGLContext::AttachShaderModule(uint32_t program, uint32_t shader)
	{
		MakeCurrent();
		glCall(m_Context.AttachShader(program, shader));
	}

	void IGLContext::LinkProgram(uint32_t program)
	{
		MakeCurrent();
		glCall(m_Context.LinkProgram(program));
	}

	int IGLContext::GetProgramiv(uint32_t program, GLenum parameter)
	{
		MakeCurrent();
		int returnValue = 0;
		glCall(m_Context.GetProgramiv(program, parameter, &returnValue));
		return returnValue;
	}

	void IGLContext::GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
	{
		MakeCurrent();
		glCall(m_Context.GetProgramInfoLog(program, maxLength, nullptr, infoLog));
	}

	void IGLContext::DetachShader(uint32_t program, uint32_t shader)
	{
		MakeCurrent();
		glCall(m_Context.DetachShader(program, shader));
	}

	void IGLContext::UseShader(uint32_t program)
	{
		MakeCurrent();
		glCall(m_Context.UseProgram(program));
	}

	// pipeline state
	void IGLContext::EnableCapability(GLenum capability, bool enable)
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

	void IGLContext::SetStencilMask(uint32_t mask)
	{
		MakeCurrent();
		glCall(m_Context.StencilMask(mask));
	}

	void IGLContext::SetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		MakeCurrent();
		glCall(m_Context.StencilOpSeparate(face, sfail, dpfail, dppass));
	}

	void IGLContext::SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
	{
		MakeCurrent();
		glCall(m_Context.StencilFuncSeparate(face, func, ref, mask));
	}

	void IGLContext::EnableDepthMask(bool enable)
	{
		MakeCurrent();
		glCall(m_Context.DepthMask(enable ? GL_TRUE : GL_FALSE));
	}

	bool IGLContext::IsDepthBoundsSupported()
	{ return m_Context.DepthBoundsEXT != nullptr; }

	void IGLContext::SetDepthBounds(float min, float max)
	{
		MakeCurrent();
		glCall(m_Context.DepthBoundsEXT(min, max));
	}

	void IGLContext::SetDepthMask(bool enabled)
	{
		MakeCurrent();
		glCall(m_Context.DepthMask(enabled ? GL_TRUE : GL_FALSE));
	}

	void IGLContext::SetDepthFunction(GLenum func)
	{
		MakeCurrent();
		glCall(m_Context.DepthFunc(func));
	}

	void IGLContext::SetFaceCulling(GLenum cullMode)
	{
		MakeCurrent();
		glCall(m_Context.CullFace(cullMode));
	}

	bool IGLContext::IsDepthClampSupported()
	{ return m_Context.EXT_depth_clamp == 1; }

	void IGLContext::SetPolygonMode(GLenum face, GLenum mode)
	{
		MakeCurrent();
		glCall(m_Context.PolygonMode(face, mode));
	}

	void IGLContext::SetFrontFace(GLenum face)
	{
		MakeCurrent();
		glCall(m_Context.FrontFace(face));
	}

	bool IGLContext::SupportsPerTargetColourMask()
	{ return m_Context.ColorMaski != nullptr; }

	void IGLContext::SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		MakeCurrent();
		glCall(m_Context.ColorMask(red, green, blue, alpha));
	}

	void IGLContext::SetColourMaski(uint32_t index, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	{
		MakeCurrent();
		glCall(m_Context.ColorMaski(index, red, green, blue, alpha));
	}

	bool IGLContext::SupportsPerTargetBlendFunction()
	{ return m_Context.BlendFunci != nullptr; }

	void IGLContext::SetBlendFunction(GLenum sfactor, GLenum dfactor)
	{
		MakeCurrent();
		glCall(m_Context.BlendFunc(sfactor, dfactor));
	}

	void IGLContext::SetBlendFunctioni(uint32_t index, GLenum sfactor, GLenum dfactor)
	{
		MakeCurrent();
		glCall(m_Context.BlendFunci(index, sfactor, dfactor));
	}

	void IGLContext::SetBlendFunctionSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha));
	}

	void IGLContext::SetBlendFunctionSeparatei(uint32_t index, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendFuncSeparatei(index, srcRGB, dstRGB, srcAlpha, dstAlpha));
	}

	void IGLContext::SetBlendEquation(GLenum mode)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquation(mode));
	}

	void IGLContext::SetBlendEquationi(uint32_t index, GLenum mode)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquationi(index, mode));
	}

	void IGLContext::SetBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquationSeparate(modeRGB, modeAlpha));
	}

	void IGLContext::SetBlendEquationSeparatei(uint32_t index, GLenum modeRGB, GLenum modeAlpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendEquationSeparatei(index, modeRGB, modeAlpha));
	}

	void IGLContext::BlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		MakeCurrent();
		glCall(m_Context.BlendColor(red, green, blue, alpha));
	}

	bool IGLContext::SupportsTextureBarriers()
	{ return m_Context.TextureBarrier != nullptr || m_Context.TextureBarrierNV != nullptr; }

	void IGLContext::TextureBarrier()
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

	bool IGLContext::SupportsMemoryBarriers()
	{ return m_Context.MemoryBarrierEXT != nullptr; }

	void IGLContext::MemoryBarrierEXT(GLbitfield barriers)
	{
		if (m_Context.MemoryBarrierEXT)
		{
			glCall(m_Context.MemoryBarrierEXT(barriers));
		}
	}

	void IGLContext::Finish()
	{ glCall(m_Context.Finish()); }

	// buffers
	uint32_t IGLContext::CreateVertexArray()
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

	void IGLContext::DestroyVertexArray(uint32_t vao)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(0));
		glCall(m_Context.DeleteVertexArrays(1, &vao));
	}

	void IGLContext::BindBuffer(GLenum target, GLuint buffer)
	{
		MakeCurrent();
		glCall(m_Context.BindBuffer(target, buffer));
	}

	void IGLContext::BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
	{
		MakeCurrent();
		glCall(m_Context.BindBufferRange(target, index, buffer, offset, size));
	}

	void IGLContext::BindVertexArray(uint32_t vao)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
	}

	void IGLContext::EnableVertexAttribArray(uint32_t vao, uint32_t index)
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

	void IGLContext::DisableVertexAttribArray(uint32_t vao, uint32_t index)
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

	void IGLContext::SetVertexAttribPointer(uint32_t  vao,
											uint32_t  vbo,
											GLuint	  index,
											GLint	  size,
											GLenum	  type,
											GLboolean normalized,
											GLsizei	  stride,
											uint32_t  offset)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void IGLContext::SetVertexAttribIPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribIPointer(index, size, type, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void IGLContext::SetVertexAttribLPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.BindBuffer(GL_ARRAY_BUFFER, vbo));
		glCall(m_Context.VertexAttribLPointer(index, size, type, stride, reinterpret_cast<const GLvoid *>(offset)));
	}

	void IGLContext::SetVertexAttribDivisor(uint32_t vao, GLuint index, GLuint divisor)
	{
		MakeCurrent();
		glCall(m_Context.BindVertexArray(vao));
		glCall(m_Context.VertexAttribDivisor(index, divisor));
	}

	bool IGLContext::AreStorageBuffersSupported()
	{ return m_Context.ARB_shader_storage_buffer_object || (m_Context.VERSION_4_5 == 1 || m_Context.ES_VERSION_3_1); }

	void IGLContext::ShaderStorageBlockBinding(uint32_t shader, GLuint storageBlockIndex, GLuint storageBlockBinding)
	{
		MakeCurrent();
		glCall(m_Context.ShaderStorageBlockBinding(shader, storageBlockIndex, storageBlockBinding));
	}

	int32_t IGLContext::GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name)
	{
		MakeCurrent();
		int32_t location = 0;
		glCall(location = m_Context.GetProgramResourceIndex(shader, programInterface, name));
		return location;
	}

	int32_t IGLContext::GetUniformBlockIndex(uint32_t shader, const GLchar *name)
	{
		MakeCurrent();
		int32_t location = 0;
		glCall(location = m_Context.GetUniformBlockIndex(shader, name));
		return location;
	}

	int32_t IGLContext::GetUniformLocation(uint32_t shader, const GLchar *name)
	{
		MakeCurrent();
		int32_t location = 0;
		glCall(location = m_Context.GetUniformLocation(shader, name));
		return location;
	}

	void IGLContext::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
		MakeCurrent();
		glCall(m_Context.UniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding));
	}

	void IGLContext::BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
	{
		MakeCurrent();
		glCall(m_Context.BindImageTexture(unit, texture, level, layered, layer, access, format));
	}

	void IGLContext::DrawArrays(GLenum mode, GLint first, GLsizei count, GLsizei primcount)
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

	void IGLContext::DrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount)
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

	void IGLContext::MultiDrawArraysIndirect(GLenum mode, const void *indirect, GLsizei drawCount, GLsizei stride)
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

	void IGLContext::MultiDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride)
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

	void IGLContext::DispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
	{
		MakeCurrent();
		glCall(m_Context.DispatchCompute(num_groups_x, num_groups_y, num_groups_z));
	}

	void IGLContext::DispatchComputeIndirect(GLintptr indirect)
	{
		MakeCurrent();
		glCall(m_Context.DispatchComputeIndirect(indirect));
	}

	void IGLContext::DrawMeshTasksEXT(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
	{
		MakeCurrent();
		if (m_Context.DrawMeshTasksEXT)
		{
			glCall(m_Context.DrawMeshTasksEXT(num_groups_x, num_groups_y, num_groups_z));
		}
	}

	void IGLContext::DrawMeshTasksIndirectEXT(GLintptr indirect, GLintptr drawCount, GLsizei stride)
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

	void IGLContext::ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
	{
		MakeCurrent();
		glCall(m_Context.ClearBufferfv(buffer, drawbuffer, value));
	}

	void IGLContext::ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
	{
		MakeCurrent();
		glCall(m_Context.ClearBufferfi(buffer, drawbuffer, depth, stencil));
	}

	void IGLContext::GetIntegerv(GLenum pname, GLint *data)
	{
		MakeCurrent();
		glCall(m_Context.GetIntegerv(pname, data));
	}

	void IGLContext::Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		MakeCurrent();
		glCall(m_Context.Scissor(x, y, width, height));
	}

	void IGLContext::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		MakeCurrent();
		glCall(m_Context.Viewport(x, y, width, height));
	}

	void IGLContext::DepthRangef(GLfloat nearVal, GLfloat farVal)
	{
		MakeCurrent();
		glCall(m_Context.DepthRangef(nearVal, farVal));
	}

	bool IGLContext::IsComputeSupported()
	{ return m_Context.DispatchCompute != nullptr; }

	bool IGLContext::IsIndirectRenderingSupported()
	{ return m_Context.DrawArraysIndirect != nullptr && m_Context.DrawElementsIndirect != nullptr; }

	bool IGLContext::IsMeshTaskSupported()
	{ return m_Context.DrawMeshTasksEXT != nullptr && m_Context.DrawMeshTasksIndirectEXT != nullptr; }

	void IGLContext::BindFramebuffer(GLenum target, GLuint framebuffer)
	{ m_Context.BindFramebuffer(target, framebuffer); }

	void IGLContext::DrawBuffers(GLuint framebuffer, GLsizei n, const GLenum *bufs)
	{
		if (m_Context.NamedFramebufferDrawBuffers)
		{
			m_Context.NamedFramebufferDrawBuffers(framebuffer, n, bufs);
		}
		else
		{
			BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
			m_Context.DrawBuffers(n, bufs);
		}
	}

	void IGLContext::ReadBuffer(GLuint framebuffer, GLenum mode)
	{
		if (m_Context.NamedFramebufferReadBuffer)
		{
			m_Context.NamedFramebufferReadBuffer(framebuffer, mode);
		}
		else
		{
			BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
			m_Context.ReadBuffer(mode);
		}
	}

	bool IGLContext::AreTimestampQueriesSupported()
	{
		return (m_Context.VERSION_3_2 == 1 || m_Context.ES_VERSION_3_0 == 1 || m_Context.ARB_timer_query == 1 ||
				m_Context.EXT_disjoint_timer_query == 1);
	}

	void IGLContext::GetTimestamp(GLint64 *data)
	{ glCall(m_Context.GetInteger64v(GL_TIMESTAMP, data)); }

	void IGLContext::DebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message)
	{
		if (m_Context.DebugMessageInsert != nullptr)
		{
			m_Context.DebugMessageInsert(source, type, id, severity, length, message);
		}
	}
}	 // namespace Nexus::GL