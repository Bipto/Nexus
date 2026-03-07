#pragma once

namespace Nexus::Graphics
{
	struct GraphicsCapabilities
	{
		bool SupportsMultisampledTextures		 = false;
		bool SupportsLODBias					 = false;
		bool SupportsInstanceOffset				 = false;
		bool SupportsMultipleSwapchains			 = false;
		bool SupportsSeparateColourAndBlendMasks = false;
	};
}	 // namespace Nexus::Graphics