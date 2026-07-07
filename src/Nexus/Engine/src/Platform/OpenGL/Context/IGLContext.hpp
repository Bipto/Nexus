#pragma once

#include <cstdint>
#include <expected>
#include <functional>

#include <glad/gl.h>

#include "RHI/IPhysicalDevice.hpp"
#include "RHI/Texture.hpp"

#include "RHI/Fence.hpp"
#include "RHI/Sampler.hpp"
#include "RHI/TexelBuffer.hpp"
#include "RHI/Texture.hpp"
#include <RHI/SamplerState.hpp>

namespace Nexus::GL
{
	class IGLContext
	{
	  public:
		virtual ~IGLContext();

		bool LoadFunctions();

		virtual bool				 MakeCurrent()		= 0;
		virtual bool				 Validate()			= 0;
		virtual const GladGLContext &GetContext() const = 0;

		// textures
		std::expected<uint32_t, std::string> CreateTexture(GLenum textureType);
		void								 TextureParameteri(GLuint texture, GLenum textureType, GLenum pname, GLint param);
		bool								 IsTextureTypeSupported(Graphics::TextureType type, uint32_t arrayLayers);

		// 1D textures
		void CompressedTexSubImage1D(GLuint		   texture,
									 GLenum		   textureType,
									 GLint		   level,
									 GLint		   xoffset,
									 GLsizei	   width,
									 GLenum		   format,
									 GLsizei	   imageSize,
									 const GLvoid *data);
		void TexSubImage1D(GLuint		 texture,
						   GLenum		 textureType,
						   GLint		 level,
						   GLint		 xoffset,
						   GLsizei		 width,
						   GLenum		 format,
						   GLenum		 type,
						   const GLvoid *pixels);

		// 2D textures
		void CompressedTexSubImage2D(GLuint		 texture,
									 GLenum		 textureType,
									 GLint		 level,
									 GLint		 xoffset,
									 GLint		 yoffset,
									 GLsizei	 width,
									 GLsizei	 height,
									 GLenum		 format,
									 GLsizei	 imageSize,
									 const void *data);
		void TexSubImage2D(GLuint		 texture,
						   GLenum		 textureType,
						   GLint		 level,
						   GLint		 xoffset,
						   GLint		 yoffset,
						   GLsizei		 width,
						   GLsizei		 height,
						   GLenum		 format,
						   GLenum		 type,
						   const GLvoid *pixels);

		// 3D textures
		void CompressedTexSubImage3D(GLuint		 texture,
									 GLenum		 textureType,
									 GLint		 level,
									 GLint		 xoffset,
									 GLint		 yoffset,
									 GLint		 zoffset,
									 GLsizei	 width,
									 GLsizei	 height,
									 GLsizei	 depth,
									 GLenum		 format,
									 GLsizei	 imageSize,
									 const void *data);
		void TexSubImage3D(GLuint	   texture,
						   GLenum	   textureType,
						   GLint	   level,
						   GLint	   xoffset,
						   GLint	   yoffset,
						   GLint	   zoffset,
						   GLsizei	   width,
						   GLsizei	   height,
						   GLsizei	   depth,
						   GLenum	   format,
						   GLenum	   type,
						   const void *pixels);

		void TexStorage1D(GLuint texture, GLenum textureType, GLsizei levels, GLenum internalformat, GLsizei width);
		void TexStorage2D(GLuint texture, GLenum textureType, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
		void TexStorage2DMultisample(GLuint	   texture,
									 GLenum	   textureType,
									 GLsizei   samples,
									 GLenum	   internalformat,
									 GLsizei   width,
									 GLsizei   height,
									 GLboolean fixedsamplelocations);
		void TexStorage3D(GLuint texture, GLenum textureType, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
		void TexStorage3DMultisample(GLuint	   texture,
									 GLenum	   textureType,
									 GLsizei   samples,
									 GLenum	   internalformat,
									 GLsizei   width,
									 GLsizei   height,
									 GLsizei   depth,
									 GLboolean fixedsamplelocations);

		bool IsSparseBindingSupported();

		// texture buffers
		std::expected<uint32_t, std::string> CreateTexelBuffer(const Graphics::TexelBufferDescription &desc);
		void								 DestroyTextureBuffer(uint32_t handle);
		void								 BindTextureBuffer(uint32_t handle, uint32_t slot);

		// buffers
		std::expected<void, std::string> CreateBuffer(GLuint			&buffer,
													  GLenum			 target,
													  GLsizeiptr		 size,
													  const GLvoid		*data,
													  GLbitfield		 mapFlags,
													  GLenum			 bufferUsage,
													  const std::string &debugName,
													  bool				&supportsPersistentMapping);

		void  DeleteBuffers(GLsizei n, const GLuint *buffers);
		void  CopyBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizei size);
		void  BufferSubData(GLuint buffer, GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
		void  GetBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid *data);
		void *MapBufferRange(GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access);

		// framebuffers
		void FramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		void FramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer);
		void FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
		void FramebufferTextureMultiviewOVR(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews);

		// samplers
		virtual std::expected<uint32_t, std::string> CreateSampler(const Graphics::SamplerDescription &desc);
		virtual void								 DestroySampler(uint32_t handle);
		virtual void								 BindSampler(uint32_t handle, uint32_t slot);

		// fences
		std::expected<GLsync, std::string> CreateFence(const Graphics::FenceDescription &desc);
		void							   DestroyFence(GLsync handle);
		bool							   IsSignalled(GLsync handle);
		GLenum							   WaitForFence(GLsync handle, uint64_t timeout);

		// shaders
		uint32_t CreateProgram();
		void	 AttachShaderModule(uint32_t program, uint32_t shader);
		void	 LinkProgram(uint32_t program);
		int		 GetProgramiv(uint32_t program, GLenum parameter);
		void	 GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
		void	 DetachShader(uint32_t program, uint32_t shader);
		void	 UseShader(uint32_t program);

		// pipeline state
		void EnableCapability(GLenum capability, bool enable);
		void SetStencilMask(uint32_t mask);
		void SetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
		void SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask);
		void EnableDepthMask(bool enable);
		bool IsDepthBoundsSupported();
		void SetDepthBounds(float min, float max);
		void SetDepthMask(bool enabled);
		void SetDepthFunction(GLenum func);
		void SetFaceCulling(GLenum cullMode);
		bool IsDepthClampSupported();
		void SetPolygonMode(GLenum face, GLenum mode);
		void SetFrontFace(GLenum face);
		bool SupportsPerTargetColourMask();
		void SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
		void SetColourMaski(uint32_t index, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
		bool SupportsPerTargetBlendFunction();
		void SetBlendFunction(GLenum sfactor, GLenum dfactor);
		void SetBlendFunctioni(uint32_t index, GLenum sfactor, GLenum dfactor);
		void SetBlendFunctionSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
		void SetBlendFunctionSeparatei(uint32_t index, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
		void SetBlendEquation(GLenum mode);
		void SetBlendEquationi(uint32_t index, GLenum mode);
		void SetBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
		void SetBlendEquationSeparatei(uint32_t index, GLenum modeRGB, GLenum modeAlpha);
		void BlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

		// synchronisation
		bool SupportsTextureBarriers();
		void TextureBarrier();
		bool SupportsMemoryBarriers();
		void MemoryBarrierEXT(GLbitfield barriers);
		void Finish();

		// buffers
		uint32_t CreateVertexArray();
		void	 DestroyVertexArray(uint32_t vao);
		void	 BindBuffer(GLenum target, GLuint buffer);
		void	 BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
		void	 BindVertexArray(uint32_t vao);
		void	 EnableVertexAttribArray(uint32_t vao, uint32_t index);
		void	 DisableVertexAttribArray(uint32_t vao, uint32_t index);
		void	 SetVertexAttribPointer(uint32_t  vao,
										uint32_t  vbo,
										GLuint	  index,
										GLint	  size,
										GLenum	  type,
										GLboolean normalized,
										GLsizei	  stride,
										uint32_t  offset);

		void SetVertexAttribIPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset);

		void SetVertexAttribLPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset);

		void SetVertexAttribDivisor(uint32_t vao, GLuint index, GLuint divisor);

		// resources
		bool	AreStorageBuffersSupported();
		void	ShaderStorageBlockBinding(uint32_t shader, GLuint storageBlockIndex, GLuint storageBlockBinding);
		int32_t GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name);
		int32_t GetUniformBlockIndex(uint32_t shader, const GLchar *name);
		int32_t GetUniformLocation(uint32_t shader, const GLchar *name);
		void	UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
		void	BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

		// draw calls
		void DrawArrays(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
		void DrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount);
		void MultiDrawArraysIndirect(GLenum mode, const void *indirect, GLsizei drawCount, GLsizei stride);
		void MultiDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);
		void DispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
		void DispatchComputeIndirect(GLintptr indirect);
		void DrawMeshTasksEXT(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
		void DrawMeshTasksIndirectEXT(GLintptr indirect, GLintptr drawCount, GLsizei stride);

		void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value);
		void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
		void GetIntegerv(GLenum pname, GLint *data);
		void Scissor(GLint x, GLint y, GLsizei width, GLsizei height);
		void Viewport(GLint x, GLint y, GLsizei width, GLsizei height);
		void DepthRangef(GLfloat nearVal, GLfloat farVal);

		bool IsComputeSupported();
		bool IsIndirectRenderingSupported();
		bool IsMeshTaskSupported();

		void BindFramebuffer(GLenum target, GLuint framebuffer);
		void DrawBuffers(GLuint framebuffer, GLsizei n, const GLenum *bufs);
		void ReadBuffer(GLuint framebuffer, GLenum mode);

		bool AreTimestampQueriesSupported();
		void GetTimestamp(GLint64 *data);

		// debug
		void DebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message);
		void ObjectLabel(GLenum identifier, GLuint name, GLsizei length, const char *label);

		void Execute(std::function<void(const GladGLContext &context)> function)
		{
			MakeCurrent();
			const GladGLContext &context = GetContext();
			function(context);
		}

	  protected:
		GladGLContext m_Context = {};
	};
}	 // namespace Nexus::GL