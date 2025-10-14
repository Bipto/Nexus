#include "TextureViewOpenGL.hpp"

namespace Nexus::Graphics
{
	TextureViewOpenGL::TextureViewOpenGL(const TextureViewDescription &desc, GraphicsDeviceOpenGL *device) : m_Description(desc), m_Device(device)
	{
		Ref<TextureOpenGL> texture = std::dynamic_pointer_cast<TextureOpenGL>(desc.TargetTexture);

		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				GLenum internalFormat = GL::GetSizedInternalFormat(desc.Format);
				GLenum m_ViewType	  = GL::GetViewType(desc);

				if (context.TextureViewEXT != nullptr)
				{
					context.GenTextures(1, &m_Handle);
					context.TextureViewEXT(m_Handle,
										   m_ViewType,
										   texture->GetHandle(),
										   internalFormat,
										   desc.Range.BaseMipLevel,
										   desc.Range.LevelCount,
										   desc.Range.BaseArrayLayer,
										   desc.Range.LayerCount);
					if (context.KHR_debug)
					{
						context.ObjectLabelKHR(GL_TEXTURE, m_Handle, -1, m_Description.DebugName.c_str());
					}
				}
			});
	}

	TextureViewOpenGL::~TextureViewOpenGL()
	{
	}

	const TextureViewDescription &TextureViewOpenGL::GetDescription() const
	{
		return m_Description;
	}

	uint32_t TextureViewOpenGL::GetHandle() const
	{
		return m_Handle;
	}

	void TextureViewOpenGL::Bind(uint32_t slot)
	{
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.ARB_direct_state_access || context.EXT_direct_state_access)
				{
					glCall(context.BindTextureUnit(slot, m_Handle));
				}
				else
				{
					glCall(context.ActiveTexture(GL_TEXTURE0 + slot));
					glCall(context.BindTexture(m_ViewType, m_Handle));
				}
			});
	}
}	 // namespace Nexus::Graphics