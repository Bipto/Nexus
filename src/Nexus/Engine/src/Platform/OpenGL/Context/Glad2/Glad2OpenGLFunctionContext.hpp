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

	  private:
		GladGLContext m_Context = {};
	};
}	 // namespace Nexus::GL