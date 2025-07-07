#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	struct PixelFormatProperties
	{
		uint32_t MaxArrayLayers = 0;
		uint32_t MaxDepth		= 0;
		uint32_t MaxMipLevels	= 0;
		uint32_t MaxWidth		= 0;
		uint32_t MaxHeight		= 0;

		bool IsSampleCountSupported(uint32_t sampleCount) const;
	};
}	 // namespace Nexus::Graphics