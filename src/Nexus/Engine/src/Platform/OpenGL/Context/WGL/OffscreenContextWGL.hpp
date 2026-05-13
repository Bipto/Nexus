#pragma once

#include "Platform/OpenGL/Context/IOffscreenContext.hpp"
#include "Platform/OpenGL/ContextDescription.hpp"

#include "Platform/Windows/WindowsInclude.hpp"
#include "glad/wgl.h"
#include <RHI/SamplerState.hpp>

namespace Nexus::GL
{
	class OffscreenContextWGL final : public IOffscreenContext
	{
	  public:
		OffscreenContextWGL(const ContextDescription &spec, Graphics::IPhysicalDevice *device);
		virtual ~OffscreenContextWGL();
		bool				 MakeCurrent() final;
		bool				 Validate() final;
		const GladGLContext &GetContext() const final;

		HGLRC GetHGLRC();

		// textures
		std::expected<uint32_t, std::string> CreateTexture(const Graphics::TextureDescription &desc) final;

		// texture buffers
		std::expected<uint32_t, std::string> CreateTexelBuffer(const Graphics::TexelBufferDescription &desc) final;
		void								 DestroyTextureBuffer(uint32_t handle) final;
		void								 BindTextureBuffer(uint32_t handle, uint32_t slot) final;

		// samplers
		std::expected<uint32_t, std::string> CreateSampler(const Graphics::SamplerDescription &desc) final;
		void								 DestroySampler(uint32_t handle) final;
		void								 BindSampler(uint32_t handle, uint32_t slot) final;

		// fences
		std::expected<GLsync, std::string> CreateFence(const Graphics::FenceDescription &desc) final;
		void							   DestroyFence(GLsync handle) final;
		bool							   IsSignalled(GLsync handle) final;
		GLenum							   WaitForFence(GLsync handle, uint64_t timeout) final;

		// shaders
		uint32_t CreateProgram() final;
		void	 AttachShaderModule(uint32_t program, uint32_t shader) final;
		void	 LinkProgram(uint32_t program) final;
		int		 GetProgramiv(uint32_t program, GLenum parameter) final;
		void	 GetProgramInfoLog(uint32_t program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) final;
		void	 DetachShader(uint32_t program, uint32_t shader) final;
		void	 UseShader(uint32_t program) final;

		// pipeline state
		void EnableCapability(GLenum capability, bool enable) final;
		void SetStencilMask(uint32_t mask) final;
		void SetStencilOp(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) final;
		void SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask) final;
		void EnableDepthMask(bool enable) final;
		bool IsDepthBoundsSupported() final;
		void SetDepthBounds(float min, float max) final;
		void SetDepthMask(bool enabled) final;
		void SetDepthFunction(GLenum func) final;
		void SetFaceCulling(GLenum cullMode) final;
		bool IsDepthClampSupported() final;
		void SetPolygonMode(GLenum face, GLenum mode) final;
		void SetFrontFace(GLenum face) final;
		bool SupportsPerTargetColourMask() final;
		void SetColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) final;
		void SetColourMaski(uint32_t index, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) final;
		bool SupportsPerTargetBlendFunction() final;
		void SetBlendFunction(GLenum sfactor, GLenum dfactor) final;
		void SetBlendFunctioni(uint32_t index, GLenum sfactor, GLenum dfactor) final;
		void SetBlendFunctionSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) final;
		void SetBlendFunctionSeparatei(uint32_t index, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) final;
		void SetBlendEquation(GLenum mode) final;
		void SetBlendEquationi(uint32_t index, GLenum mode) final;
		void SetBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) final;
		void SetBlendEquationSeparatei(uint32_t index, GLenum modeRGB, GLenum modeAlpha) final;

		// buffers
		uint32_t CreateVertexArray() final;
		void	 DestroyVertexArray(uint32_t vao) final;
		void	 BindBuffer(GLenum target, GLuint buffer) final;
		void	 BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) final;
		void	 BindVertexArray(uint32_t vao) final;
		void	 EnableVertexAttribArray(uint32_t vao, uint32_t index) final;
		void	 DisableVertexAttribArray(uint32_t vao, uint32_t index) final;
		void	 SetVertexAttribPointer(uint32_t  vao,
										uint32_t  vbo,
										GLuint	  index,
										GLint	  size,
										GLenum	  type,
										GLboolean normalized,
										GLsizei	  stride,
										uint32_t  offset) final;

		void SetVertexAttribIPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset) final;

		void SetVertexAttribLPointer(uint32_t vao, uint32_t vbo, GLuint index, GLint size, GLenum type, GLsizei stride, uint32_t offset) final;

		void SetVertexAttribDivisor(uint32_t vao, GLuint index, GLuint divisor) final;

		// resources
		bool	AreStorageBuffersSupported() final;
		int32_t GetProgramResourceIndex(uint32_t shader, GLenum programInterface, const char *name) final;
		int32_t GetUniformBlockIndex(uint32_t shader, const GLchar *name) final;
		int32_t GetUniformLocation(uint32_t shader, const GLchar *name) final;
		void	UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) final;
		void	BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) final;

	  private:
		std::tuple<HPBUFFERARB, HDC, HGLRC> CreatePBufferContext(HDC hdc, const ContextDescription &spec);

	  private:
		HGLRC		m_HGLRC	  = {};
		HPBUFFERARB m_PBuffer = {};
		HDC			m_HDC	  = {};

		OpenGLFunctionContext m_FunctionContext = {};

	  private:
		inline static bool s_GLFunctionsLoaded = false;
	};
}	 // namespace Nexus::GL