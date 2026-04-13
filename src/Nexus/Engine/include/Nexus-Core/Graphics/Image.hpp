#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "RHI/PixelFormat.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::Graphics
{
	class IGraphicsDevice;
	class ICommandQueue;

	struct Image
	{
		uint32_t		  Width	 = 0;
		uint32_t		  Height = 0;
		std::vector<char> Pixels = {};
		PixelFormat		  Format = PixelFormat::R8_G8_B8_A8_UNorm;

		void FlipVertically();

		static Image FromTexture(IGraphicsDevice   *device,
								 Ref<ICommandQueue> commandQueue,
								 TextureHandle		texture,
								 uint32_t			arrayLayer,
								 uint32_t			mipLevel,
								 uint32_t			x,
								 uint32_t			y,
								 uint32_t			z,
								 uint32_t			width,
								 uint32_t			height);
	};
}	 // namespace Nexus::Graphics