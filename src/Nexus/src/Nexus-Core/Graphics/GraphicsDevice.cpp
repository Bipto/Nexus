#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/Graphics/ShaderGenerator.hpp"
#include "Nexus-Core/Graphics/ShaderUtils.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "Platform/FileSystem/FileSystem.hpp"
#include "Platform/Logging/Log.hpp"

#include "Nexus-Core/Caching/CachedShader.hpp"

namespace Nexus::Graphics
{

	Ref<IFramebuffer> IGraphicsDevice::CreateFramebuffer(const FramebufferTextureCreateDescription &desc)
	{
		FramebufferTextureSetDescription framebufferDesc = {};

		for (size_t i = 0; i < desc.ColourAttachmentFormats.size(); i++)
		{
			PixelFormat format = desc.ColourAttachmentFormats.at(i);

			TextureDescription textureDesc = {};
			textureDesc.Width			   = desc.Width;
			textureDesc.Height			   = desc.Height;
			textureDesc.Type			   = TextureType::Texture2D;
			textureDesc.Usage			   = Graphics::TextureUsage_ColourAttachment;
			textureDesc.Samples			   = desc.Samples;
			textureDesc.Format			   = format;

			Ref<ITexture> colourAttachment = CreateTexture(textureDesc);

			FramebufferColourAttachmentDescription &framebufferTextureDesc = framebufferDesc.ColourAttachments.emplace_back();
			framebufferTextureDesc.ColourAttachment.TargetTexture		   = colourAttachment;
			framebufferTextureDesc.ColourAttachment.BaseArrayLayer		   = 0;
			framebufferTextureDesc.ColourAttachment.LayerCount			   = 1;
			framebufferTextureDesc.ColourAttachment.MipLevel			   = 0;

			// create a resolve attachment if using multi-sampling
			if (textureDesc.Samples > 1)
			{
				TextureDescription resolveTextureDesc = {};
				resolveTextureDesc.Width			  = desc.Width;
				resolveTextureDesc.Height			  = desc.Height;
				resolveTextureDesc.Type				  = TextureType::Texture2D;
				resolveTextureDesc.Usage			  = Graphics::TextureUsage_ColourAttachment;
				resolveTextureDesc.Samples			  = 1;
				resolveTextureDesc.Format			  = format;

				Ref<ITexture> resolveAttachment = CreateTexture(resolveTextureDesc);

				FramebufferTextureDescription resolveAttachmentDesc = {};
				resolveAttachmentDesc.TargetTexture					= resolveAttachment;
				resolveAttachmentDesc.BaseArrayLayer				= 0;
				resolveAttachmentDesc.LayerCount					= 1;
				resolveAttachmentDesc.MipLevel						= 0;
				framebufferTextureDesc.ResolveAttachment			= resolveAttachmentDesc;
			}
		}

		if (desc.DepthAttachmentFormat.has_value())
		{
			TextureDescription textureDesc = {};
			textureDesc.Width			   = desc.Width;
			textureDesc.Height			   = desc.Height;
			textureDesc.Type			   = TextureType::Texture2D;
			textureDesc.Usage			   = Graphics::TextureUsage_DepthStencilAttachment;
			textureDesc.Samples			   = desc.Samples;
			textureDesc.Format			   = desc.DepthAttachmentFormat.value();

			Ref<ITexture> texture = CreateTexture(textureDesc);

			FramebufferTextureDescription framebufferTextureDesc = {};
			framebufferTextureDesc.TargetTexture				 = texture;
			framebufferTextureDesc.BaseArrayLayer				 = 0;
			framebufferTextureDesc.LayerCount					 = 1;
			framebufferTextureDesc.MipLevel						 = 0;

			framebufferDesc.DepthAttachment = framebufferTextureDesc;
		}

		return CreateFramebuffer(framebufferDesc);
	}
}	 // namespace Nexus::Graphics
