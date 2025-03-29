#pragma once

#include "Nexus-Core/Graphics/PixelFormat.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct Image
	{
		uint32_t				   Width  = 0;
		uint32_t				   Height = 0;
		std::vector<unsigned char> Pixels = {};
		PixelFormat				   Format = PixelFormat::R8_G8_B8_A8_UNorm;

		void FlipVertically();
	};
}	 // namespace Nexus::Graphics