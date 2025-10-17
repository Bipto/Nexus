#include "Nexus-Core/Graphics/Framebuffer.hpp"

namespace Nexus::Graphics
{
	bool Nexus::Graphics::FramebufferTextureSetDescription::ValidateSamples() const
	{
		// storage for initial sample count to test against
		uint32_t testSampleCount = 0;

		// we need to retrieve the first valid texture from the framebuffer so that we can retrieve it's sample count
		if (ColourAttachments.size() > 0)
		{
			testSampleCount = ColourAttachments[0].ColourAttachment.TargetTexture->GetSampleCount();
		}
		// if there are no colour attachment, we try to get the depth attachment
		else if (DepthAttachment.has_value())
		{
			testSampleCount = DepthAttachment.value().TargetTexture->GetSampleCount();
		}
		// Otherwise, there are no valid attachments
		else
		{
			return false;
		}

		// iterate through all colour attachments and test their sample counts
		for (const auto &colourAttachment : ColourAttachments)
		{
			// the sample count does not match the first texture's so this set is invalid
			if (colourAttachment.ColourAttachment.TargetTexture->GetSampleCount() != testSampleCount)
			{
				return false;
			}

			// each resolve attachment must have a sample count of 1
			if (colourAttachment.ResolveAttachment.has_value() && colourAttachment.ResolveAttachment.value().TargetTexture->GetSampleCount() != 1)
			{
				return false;
			}
		}

		// test against the depth attachment if one exists
		if (DepthAttachment.has_value())
		{
			// the depth textures sample count does not match the first texture's so this set is invalid
			Ref<Texture> texture = DepthAttachment.value().TargetTexture;
			if (texture->GetSampleCount() != testSampleCount)
			{
				return false;
			}
		}

		// we have a valid framebuffer
		return true;
	}

	bool Nexus::Graphics::FramebufferTextureSetDescription::ValidateDimensions() const
	{
		// storage for initial width and height to test against
		uint32_t testWidth	= 0;
		uint32_t testHeight = 0;

		// we need to retrieve the first valid texture from the framebuffer so that we can retrieve it's width and height
		if (ColourAttachments.size() > 0)
		{
			Ref<Texture> colourAttachment = ColourAttachments[0].ColourAttachment.TargetTexture;
			testWidth					  = colourAttachment->GetWidth();
			testHeight					  = colourAttachment->GetHeight();
		}
		// if there are no colour attachment, we try to get the depth attachment
		else if (DepthAttachment.has_value())
		{
			Ref<Texture> texture = DepthAttachment.value().TargetTexture;
			testWidth			 = texture->GetWidth();
			testHeight			 = texture->GetHeight();
		}
		// Otherwise, there are no valid attachments
		else
		{
			return false;
		}

		// iterate through all colour attachments and test their widths and heights
		for (const auto &colourAttachment : ColourAttachments)
		{
			Ref<Texture> colourTexture = colourAttachment.ColourAttachment.TargetTexture;

			// the dimensions of this texture does not match the first texture's so this set is invalid
			if (colourTexture->GetWidth() != testWidth || colourTexture->GetHeight() != testHeight)
			{
				return false;
			}

			if (colourAttachment.ResolveAttachment.has_value())
			{
				if (Ref<Texture> resolveTexture = colourAttachment.ResolveAttachment.value().TargetTexture)
				{
					// the dimensions of this texture does not match the first texture's so this set is invalid
					if (resolveTexture->GetWidth() != testWidth || resolveTexture->GetHeight() != testHeight)
					{
						return false;
					}
				}
			}
		}

		// test against the depth attachment if one exists
		if (DepthAttachment.has_value())
		{
			Ref<Texture> texture = DepthAttachment.value().TargetTexture;

			// the depth textures dimensions does not match the first texture's so this set is invalid
			if (texture->GetWidth() != testWidth || texture->GetHeight() != testHeight)
			{
				return false;
			}
		}

		// we have a valid framebuffer
		return true;
	}

	bool Nexus::Graphics::FramebufferTextureSetDescription::ValidateHasTexture() const
	{
		// checks whether there are any colour attachments or a depth attachment in this framebuffer
		return ColourAttachments.size() > 0 || DepthAttachment.has_value();
	}

	bool FramebufferTextureSetDescription::ValidateUsageFlags() const
	{
		// there are no textures to check
		if (!ValidateHasTexture())
		{
			return false;
		}

		// iterate through all colour attachments
		for (const auto &colourAttachment : ColourAttachments)
		{
			// a texture that is being in a framebuffer as a colour attachment must have the ColourAttachment usage flag set
			Ref<Texture> colourTexture = colourAttachment.ColourAttachment.TargetTexture;
			if (!(colourTexture->GetUsage() & Graphics::TextureUsage_ColourAttachment))
			{
				return false;
			}

			if (colourAttachment.ResolveAttachment.has_value())
			{
				if (Ref<Texture> resolveTexture = colourAttachment.ResolveAttachment.value().TargetTexture)
				{
					// a texture that is being in a framebuffer as a resolve attachment must have the ColourAttachment usage flag set
					if (!(resolveTexture->GetUsage() & Graphics::TextureUsage_ColourAttachment))
					{
						return false;
					}
				}
			}
		}

		// check the depth attachment if one exists
		if (DepthAttachment.has_value())
		{
			// a texture that is being in a framebuffer as a depth/stencil attachment must have the DepthStencilAttachment usage flag set
			Ref<Texture> texture = DepthAttachment.value().TargetTexture;
			if (!(texture->GetUsage() & Graphics::TextureUsage_DepthStencilAttachment))
			{
				return false;
			}
		}

		// all textures have valid usage flags
		return true;
	}

	uint32_t FramebufferTextureSetDescription::GetSampleCount() const
	{
		// we first try to retrieve the sample count from a colour attachment
		// technically, this doesn't need to be a loop, but this is simpler
		for (const auto &colourAttachment : ColourAttachments)
		{
			Ref<Texture> texture = colourAttachment.ColourAttachment.TargetTexture;
			return texture->GetSampleCount();
		}

		// otherwise we try to retrieve the sample count from the depth/stencil attachment
		if (DepthAttachment.has_value())
		{
			Ref<Texture> texture = DepthAttachment.value().TargetTexture;
			return texture->GetSampleCount();
		}

		// otherwise,we do not have any valid textures
		return 0;
	}

	Nexus::Point2D<uint32_t> FramebufferTextureSetDescription::GetSize() const
	{
		// we first try to retrieve the sample count from a colour attachment
		// technically, this doesn't need to be a loop, but this is simpler
		for (const auto &colourAttachment : ColourAttachments)
		{
			Ref<Texture> texture = colourAttachment.ColourAttachment.TargetTexture;

			Nexus::Point2D<uint32_t> size = {};
			size.X						  = texture->GetWidth();
			size.Y						  = texture->GetHeight();
			return size;
		}

		// otherwise we try to retrieve the sample count from the depth/stencil attachment
		if (DepthAttachment.has_value())
		{
			Ref<Texture>			 texture = DepthAttachment.value().TargetTexture;
			Nexus::Point2D<uint32_t> size	 = {};
			size.X							 = texture->GetWidth();
			size.Y							 = texture->GetHeight();
			return size;
		}

		// otherwise,we do not have any valid textures
		return {0, 0};
	}
}	 // namespace Nexus::Graphics