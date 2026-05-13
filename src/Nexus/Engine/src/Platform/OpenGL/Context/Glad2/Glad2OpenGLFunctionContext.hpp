#pragma once

#include <expected>
#include <functional>

#include "glad/gl.h"

#include "RHI/Fence.hpp"
#include "RHI/TexelBuffer.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::GL
{
	class OpenGLFunctionContext
	{
	  public:
		OpenGLFunctionContext();
		virtual ~OpenGLFunctionContext();

		bool Load();

		void ExecuteCommands(std::function<void(const GladGLContext &context)> function);

		const GladGLContext &GetContext() const;

		// textures
		std::expected<uint32_t, std::string> CreateTexture(const Graphics::TextureDescription &desc);

		// texel buffers
		std::expected<uint32_t, std::string> CreateTexelBuffer(const Graphics::TexelBufferDescription &desc);
		void								 DestroyTextureBuffer(uint32_t handle);
		void								 BindTextureBuffer(uint32_t handle, uint32_t slot);

		// samplers
		std::expected<uint32_t, std::string> CreateSampler(const Graphics::SamplerDescription &desc);
		void								 DestroySampler(uint32_t handle);
		void								 BindSampler(uint32_t handle, uint32_t slot);

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
		int32_t GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name);
		int32_t GetUniformBlockIndex(uint32_t shader, const GLchar *name);
		int32_t GetUniformLocation(uint32_t shader, const GLchar *name);
		void	UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
		void	BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);

	  private:
		GladGLContext m_Context = {};
	};
}	 // namespace Nexus::GL