#include "TextureViewOpenGL.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	bool IsFullTextureView(const TextureViewDescription &desc)
	{
		const TextureDescription &textureDesc = desc.TargetTexture->GetDescription();
		return textureDesc.Format == desc.Format && desc.Range.BaseMipLevel == 0 && desc.Range.BaseArrayLayer == 0 &&
			   desc.Range.LayerCount == textureDesc.DepthOrArrayLayers && desc.Range.LevelCount == textureDesc.MipLevels;
	}

	TextureViewOpenGL::TextureViewOpenGL(const TextureViewDescription &desc, GraphicsDeviceOpenGL *device) : m_Description(desc), m_Device(device)
	{
		const TextureDescription &textureDesc = m_Description.TargetTexture->GetDescription();

		// if the texture view matches the full texture, we can use the texture directly skipping the view
		m_TextureViewRequired = !IsFullTextureView(desc);

		if (m_TextureViewRequired)
		{
			GL::ExecuteGLCommands(
				[&](const GladGLContext &context)
				{
					const bool textureViewSupported = context.TextureViewEXT != nullptr;

					if (textureViewSupported)
					{
						CreateTextureView(context);
					}
					else
					{
						CreateEmulatedView(context);
					}
				});
		}
	}

	TextureViewOpenGL::~TextureViewOpenGL()
	{
		if (m_Handle)
		{
			GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteTextures(1, &m_Handle); });
		}
	}

	const TextureViewDescription &TextureViewOpenGL::GetDescription() const
	{
		return m_Description;
	}

	const uint32_t TextureViewOpenGL::GetHandle() const
	{
		return m_Handle;
	}

	void TextureViewOpenGL::Bind(uint32_t slot) const
	{
		if (m_TextureViewRequired)
		{
			GL::ExecuteGLCommands(
				[&](const GladGLContext &context)
				{
					const bool textureViewSupported = context.TextureViewEXT != nullptr;

					// we have a valid texture view
					if (m_Handle)
					{
						if (m_Dirty && !textureViewSupported)
						{
							UpdateEmulatedView(context);
						}

						// if we have texture view support, bind normally
						if (textureViewSupported)
						{
							// try to use DSA if available
							if (context.ARB_direct_state_access || context.EXT_direct_state_access)
							{
								glCall(context.BindTextureUnit(slot, m_Handle));
							}
							else
							{
								glCall(context.ActiveTexture(GL_TEXTURE0 + slot));
								glCall(context.BindTexture(m_ViewType, m_Handle));
							}
						}
						// otherwise bind the emulated texture view
						else
						{
							m_EmulatedTextureView->Bind(slot);
						}
					}
				});
		}
		else
		{
			Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(m_Description.TargetTexture);
			texture->Bind(slot);
		}

		m_Dirty = false;
	}

	void TextureViewOpenGL::MarkDirty() const
	{
		m_Dirty = true;
	}

	void TextureViewOpenGL::CreateTextureView(const GladGLContext &context)
	{
		Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(m_Description.TargetTexture);

		GLenum internalFormat = GL::GetSizedInternalFormat(m_Description.Format);
		GLenum m_ViewType	  = GL::GetViewType(m_Description);

		context.GenTextures(1, &m_Handle);
		context.TextureViewEXT(m_Handle,
							   m_ViewType,
							   texture->GetHandle(),
							   internalFormat,
							   m_Description.Range.BaseMipLevel,
							   m_Description.Range.LevelCount,
							   m_Description.Range.BaseArrayLayer,
							   m_Description.Range.LayerCount);
		if (context.KHR_debug)
		{
			context.ObjectLabelKHR(GL_TEXTURE, m_Handle, -1, m_Description.DebugName.c_str());
		}
	}

	void TextureViewOpenGL::CreateEmulatedView(const GladGLContext &context)
	{
		Point2D<uint32_t> mipSize = Utils::GetMipSize(m_Description.TargetTexture->GetDescription().Width,
													  m_Description.TargetTexture->GetDescription().Height,
													  m_Description.Range.BaseMipLevel);

		const TextureDescription &originalDesc = m_Description.TargetTexture->GetDescription();

		TextureDescription textureDesc = {};
		textureDesc.Type			   = originalDesc.Type;
		textureDesc.Format			   = m_Description.Format;
		textureDesc.Samples			   = originalDesc.Samples;
		textureDesc.Width			   = mipSize.X;
		textureDesc.Height			   = mipSize.Y;
		textureDesc.DepthOrArrayLayers = m_Description.Range.LayerCount;
		textureDesc.MipLevels		   = m_Description.Range.LevelCount;
		textureDesc.Usage			   = originalDesc.Usage;
		textureDesc.DebugName		   = m_Description.DebugName;

		m_EmulatedTextureView = CreateRef<TextureOpenGL>(textureDesc, m_Device);
		m_Handle			  = m_EmulatedTextureView->GetHandle();
	}

	void TextureViewOpenGL::UpdateEmulatedView(const GladGLContext &context) const
	{
		Ref<TextureOpenGL> source = std::dynamic_pointer_cast<TextureOpenGL>(m_Description.TargetTexture);

		for (uint32_t arrayLayer = m_Description.Range.BaseArrayLayer;
			 arrayLayer < m_Description.Range.BaseArrayLayer + m_Description.Range.LayerCount;
			 arrayLayer++)
		{
			for (uint32_t mip = m_Description.Range.BaseMipLevel; mip < m_Description.Range.BaseMipLevel + m_Description.Range.LevelCount; mip++)
			{
				auto [mipWidth, mipHeight] =
					Utils::GetMipSize(m_Description.TargetTexture->GetDescription().Width, m_Description.TargetTexture->GetDescription().Height, mip);

				Graphics::TextureCopyDescription copyDesc = {};
				copyDesc.Source							  = m_Description.TargetTexture;
				copyDesc.SourceOffset					  = {0, 0, (int32_t)arrayLayer};
				copyDesc.SourceMipLevel					  = mip;
				copyDesc.Destination					  = m_EmulatedTextureView;
				copyDesc.DestinationOffset				  = {0, 0, (int32_t)arrayLayer};
				copyDesc.DestinationMipLevel			  = mip - m_Description.Range.BaseMipLevel;
				copyDesc.Extent							  = {mipWidth, mipHeight};

				GL::CopyTextureToTexture(copyDesc, context);
			}
		}
	}

}	 // namespace Nexus::Graphics