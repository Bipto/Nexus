#include "TexelBufferOpenGL.hpp"

namespace Nexus::Graphics
{
	Nexus::Graphics::TexelBufferOpenGL::TexelBufferOpenGL(const TexelBufferDescription &desc, GraphicsDeviceOpenGL *device)
	{
		GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
		m_Handle					   = context->CreateTexelBuffer(desc).value();
	}

	TexelBufferOpenGL::~TexelBufferOpenGL()
	{
		GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
		context->DestroyTextureBuffer(m_Handle);
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
		GL::IOffscreenContext *context = m_Device->GetOffscreenContext();
		context->BindTextureBuffer(m_Handle, slot);
	}

}	 // namespace Nexus::Graphics
