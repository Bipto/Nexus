#pragma once

#include <expected>
#include <string>

#include "Platform/OpenGL/GL.hpp"

#include "IGLContext.hpp"
#include "RHI/IPhysicalDevice.hpp"

#include "RHI/Fence.hpp"
#include "RHI/Sampler.hpp"
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
	};
}	 // namespace Nexus::GL