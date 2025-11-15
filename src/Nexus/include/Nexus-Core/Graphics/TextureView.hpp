#pragma once

#include "Nexus-Core/Graphics/Structures.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
	struct TextureViewDescription
	{
		Ref<ITexture>	 TargetTexture = nullptr;
		PixelFormat		 Format		   = PixelFormat::Invalid;
		SubresourceRange Range		   = {};
		std::string		 DebugName	   = "TextureView";
	};

	class ITextureView
	{
	  public:
		virtual ~ITextureView()										 = default;
		virtual const TextureViewDescription &GetDescription() const = 0;
		Ref<ITexture>						  GetTexture() const
		{
			return GetDescription().TargetTexture;
		}
	};
}	 // namespace Nexus::Graphics