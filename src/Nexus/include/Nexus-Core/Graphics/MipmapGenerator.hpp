#pragma once

#include "Nexus-Core/Graphics/Image.hpp"

namespace Nexus::Graphics
{
	class MipmapGenerator
	{
	  public:
		MipmapGenerator() = default;
		std::vector<Image> GenerateMipChain(Image baseImage);
		Image			   GenerateMip(const Image &baseImage, uint32_t level);
		static uint32_t	   GetMaximumNumberOfMips(uint32_t width, uint32_t height);
	};
}	 // namespace Nexus::Graphics