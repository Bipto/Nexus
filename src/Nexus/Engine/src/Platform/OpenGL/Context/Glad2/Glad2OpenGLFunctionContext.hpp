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
		void EnableDepthTest(bool enable);
		void EnableDepthWrite(bool enable);
		void EnableStencilTest(bool enable);
		void SetStencilReference(uint32_t reference);

	  private:
		GladGLContext m_Context = {};
	};
}	 // namespace Nexus::GL