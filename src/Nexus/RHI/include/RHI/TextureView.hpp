#pragma once

#include "Core/ResourcePool.hpp"

#include "RHI/Structures.hpp"
#include "RHI/Texture.hpp"

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

	struct TextureViewTag
	{
	};
	using TextureViewID		= Nexus::HandleT<TextureViewTag>;
	using TextureViewHandle = SharedHandle<ITextureView, TextureViewID>;
	using TextureViewPool	= ResourcePool<ITextureView, TextureViewID>;
}	 // namespace Nexus::Graphics