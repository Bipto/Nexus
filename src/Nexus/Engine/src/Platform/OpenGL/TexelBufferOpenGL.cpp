#include "TexelBufferOpenGL.hpp"

namespace Nexus::Graphics
{
	Nexus::Graphics::TexelBufferOpenGL::TexelBufferOpenGL(const TexelBufferDescription &desc, GraphicsDeviceOpenGL *device)
	{
		GLenum					internalFormat = GL::GetSizedInternalFormat(desc.Format);
		Ref<DeviceBufferOpenGL> buffer		   = std::dynamic_pointer_cast<DeviceBufferOpenGL>(desc.Buffer);

		NX_VALIDATE(buffer, "Invalid buffer supplied when attempting to create texel buffer");

		GL::SetCurrentContext(m_Device->GetOffscreenContext());
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				if (context.EXT_direct_state_access || context.ARB_direct_state_access && context.TextureBufferRange)
				{
					context.CreateTextures(GL_TEXTURE_BUFFER, 1, &m_Handle);
					context.TextureBufferRange(m_Handle, internalFormat, buffer->GetHandle(), desc.Offset, desc.SizeInBytes);
				}
				else if (context.TexBufferRangeEXT)
				{
					context.GenTextures(1, &m_Handle);
					context.BindTexture(GL_TEXTURE_BUFFER, m_Handle);
					context.TexBufferRangeEXT(GL_TEXTURE_BUFFER, internalFormat, buffer->GetHandle(), desc.Offset, desc.SizeInBytes);
				}
			});
	}

	TexelBufferOpenGL::~TexelBufferOpenGL()
	{
		GL::SetCurrentContext(m_Device->GetOffscreenContext());
		GL::ExecuteGLCommands([&](const GladGLContext &context) {});
	}

	const TexelBufferDescription &TexelBufferOpenGL::GetDescription() const
	{
		return m_Description;
	}

	const uint32_t TexelBufferOpenGL::GetTexelBufferHandle() const
	{
		return m_Handle;
	}

	void TexelBufferOpenGL::Bind(uint32_t slot) const
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
					glCall(context.BindTexture(GL_TEXTURE_BUFFER, m_Handle));
				}
			});
	}

}	 // namespace Nexus::Graphics
