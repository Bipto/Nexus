#pragma once

#include <expected>
#include <string>

#include "Platform/OpenGL/GL.hpp"

#include "IGLContext.hpp"
#include "RHI/IPhysicalDevice.hpp"

#include "RHI/Fence.hpp"
#include "RHI/Sampler.hpp"
#include "RHI/TexelBuffer.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::GL
{
	class IOffscreenContext : public IGLContext
	{
	  public:
		virtual ~IOffscreenContext() = default;

		// textures
		virtual std::expected<uint32_t, std::string> CreateTexture(const Graphics::TextureDescription &desc) = 0;

		// texture buffers
		virtual std::expected<uint32_t, std::string> CreateTexelBuffer(const Graphics::TexelBufferDescription &desc) = 0;
		virtual void								 DestroyTextureBuffer(uint32_t handle)							 = 0;
		virtual void								 BindTextureBuffer(uint32_t handle, uint32_t slot)				 = 0;

		// samplers
		virtual std::expected<uint32_t, std::string> CreateSampler(const Graphics::SamplerDescription &desc) = 0;
		virtual void								 DestroySampler(uint32_t handle)						 = 0;
		virtual void								 BindSampler(uint32_t handle, uint32_t slot)			 = 0;

		// fences
		virtual std::expected<GLsync, std::string> CreateFence(const Graphics::FenceDescription &desc) = 0;
		virtual void							   DestroyFence(GLsync handle)						   = 0;
		virtual bool							   IsSignalled(GLsync handle)						   = 0;
		virtual GLenum							   WaitForFence(GLsync handle, uint64_t timeout)	   = 0;

		// shaders
		virtual uint32_t CreateProgram()																		  = 0;
		virtual void	 AttachShaderModule(uint32_t program, uint32_t shader)									  = 0;
		virtual void	 LinkProgram(uint32_t program)															  = 0;
		virtual int		 GetProgramiv(uint32_t program, GLenum parameter)										  = 0;
		virtual void	 GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) = 0;
		virtual void	 DetachShader(uint32_t program, uint32_t shader)										  = 0;
		virtual void	 UseShader(uint32_t program)															  = 0;

		// pipeline state
		virtual void EnableCapability(GLenum capability, bool enable)														   = 0;
		virtual void SetStencilMask(uint32_t mask)																			   = 0;
		virtual void SetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)									   = 0;
		virtual void SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)										   = 0;
		virtual void EnableDepthMask(bool enable)																			   = 0;
		virtual bool IsDepthBoundsSupported()																				   = 0;
		virtual void SetDepthBounds(float min, float max)																	   = 0;
		virtual void SetDepthMask(bool enabled)																				   = 0;
		virtual void SetDepthFunction(GLenum func)																			   = 0;
		virtual void SetFaceCulling(GLenum cullMode)																		   = 0;
		virtual bool IsDepthClampSupported()																				   = 0;
		virtual void SetPolygonMode(GLenum face, GLenum mode)																   = 0;
		virtual void SetFrontFace(GLenum face)																				   = 0;
		virtual bool SupportsPerTargetColourMask()																			   = 0;
		virtual void SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)							   = 0;
		virtual void SetColourMaski(uint32_t index, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)		   = 0;
		virtual bool SupportsPerTargetBlendFunction()																		   = 0;
		virtual void SetBlendFunction(GLenum sfactor, GLenum dfactor)														   = 0;
		virtual void SetBlendFunctioni(uint32_t index, GLenum sfactor, GLenum dfactor)										   = 0;
		virtual void SetBlendFunctionSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)				   = 0;
		virtual void SetBlendFunctionSeparatei(uint32_t index, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) = 0;
		virtual void SetBlendEquation(GLenum mode)																			   = 0;
		virtual void SetBlendEquationi(uint32_t index, GLenum mode)															   = 0;
		virtual void SetBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)												   = 0;
		virtual void SetBlendEquationSeparatei(uint32_t index, GLenum modeRGB, GLenum modeAlpha)							   = 0;

		// buffers
		virtual uint32_t CreateVertexArray()																		   = 0;
		virtual void	 DestroyVertexArray(uint32_t vao)															   = 0;
		virtual void	 BindBuffer(GLenum target, GLuint buffer)													   = 0;
		virtual void	 BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) = 0;
		virtual void	 BindVertexArray(uint32_t vao)																   = 0;
		virtual void	 EnableVertexAttribArray(uint32_t vao, uint32_t index)										   = 0;
		virtual void	 DisableVertexAttribArray(uint32_t vao, uint32_t index)										   = 0;
		virtual void	 SetVertexAttribPointer(uint32_t  vao,
												uint32_t  vbo,
												GLuint	  index,
												GLint	  size,
												GLenum	  type,
												GLboolean normalized,
												GLsizei	  stride,
												uint32_t  offset)													   = 0;

		virtual void SetVertexAttribIPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset) = 0;

		virtual void SetVertexAttribLPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset) = 0;

		virtual void SetVertexAttribDivisor(uint32_t vao, GLuint index, GLuint divisor) = 0;

		// resources
		virtual bool	AreStorageBuffersSupported()																							 = 0;
		virtual void	ShaderStorageBlockBinding(uint32_t shader, GLuint storageBlockIndex, GLuint storageBlockBinding)						 = 0;
		virtual int32_t GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name)										 = 0;
		virtual int32_t GetUniformBlockIndex(uint32_t shader, const GLchar *name)																 = 0;
		virtual int32_t GetUniformLocation(uint32_t shader, const GLchar *name)																	 = 0;
		virtual void	UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)								 = 0;
		virtual void	BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) = 0;

		// draw calls
		virtual void DrawArrays(GLenum mode, GLint first, GLsizei count, GLsizei primcount)										  = 0;
		virtual void DrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount)				  = 0;
		virtual void MultiDrawArraysIndirect(GLenum mode, const void *indirect, GLsizei drawCount, GLsizei stride)				  = 0;
		virtual void MultiDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride) = 0;
		virtual void DispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)								  = 0;
		virtual void DispatchComputeIndirect(GLintptr indirect)																	  = 0;
		virtual void MemoryBarrierEXT(GLbitfield barriers)																		  = 0;
		virtual void TextureBarrier()																							  = 0;
		virtual void DrawMeshTasksEXT(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)							  = 0;
		virtual void DrawMeshTasksIndirectEXT(GLintptr indirect, GLintptr drawCount, GLsizei stride)							  = 0;

		virtual void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)		  = 0;
		virtual void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) = 0;
		virtual void GetIntegerv(GLenum pname, GLint *data)										  = 0;
		virtual void Scissor(GLint x, GLint y, GLsizei width, GLsizei height)					  = 0;
		virtual void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)					  = 0;
		virtual void DepthRangef(GLfloat nearVal, GLfloat farVal)								  = 0;
	};
}	 // namespace Nexus::GL