#pragma once

#include "Nexus-Core/Graphics/PixelFormat.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class GraphicsDevice;
	class Texture;

	struct Image
	{
		uint32_t				   Width  = 0;
		uint32_t				   Height = 0;
		std::vector<char>		   Pixels = {};
		PixelFormat				   Format = PixelFormat::R8_G8_B8_A8_UNorm;

		void FlipVertically();

		static Image FromTexture(GraphicsDevice *device,
								 Ref<Texture>	 texture,
								 uint32_t		 arrayLayer,
								 uint32_t		 mipLevel,
								 uint32_t		 x,
								 uint32_t		 y,
								 uint32_t		 z,
								 uint32_t		 width,
								 uint32_t		 height);
	};
}	 // namespace Nexus::Graphics