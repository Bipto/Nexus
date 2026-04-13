#pragma once

#include "Core/ResourcePool.hpp"

#include "RHI/Structures.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::Graphics
{
	struct TextureViewDescription
	{
		TextureHandle	 TargetTexture = {};
		PixelFormat		 Format		   = PixelFormat::Invalid;
		SubresourceRange Range		   = {};
		std::string		 DebugName	   = "TextureView";
	};

	class ITextureView
	{
	  public:
		virtual ~ITextureView()										 = default;
		virtual const TextureViewDescription &GetDescription() const = 0;
		TextureHandle						  GetTexture() const
		{
			return GetDescription().TargetTexture;
		}
	};

	DEFINE_RESOURCE(TextureView, ITextureView);
}	 // namespace Nexus::Graphics