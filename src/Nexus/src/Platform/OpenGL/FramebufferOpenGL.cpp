#if defined(NX_PLATFORM_OPENGL)

	#include "FramebufferOpenGL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"
	#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
	FramebufferOpenGL::FramebufferOpenGL(const FramebufferSpecification &spec, GraphicsDeviceOpenGL *graphicsDevice)
		: Framebuffer(spec),
		  m_Device(graphicsDevice)
	{
		Recreate();
	}

	FramebufferOpenGL::~FramebufferOpenGL()
	{
		DeleteTextures();
	}

	void FramebufferOpenGL::BindAsRenderTarget()
	{
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));

		auto width	= m_Specification.Width;
		auto height = m_Specification.Height;
		glCall(glViewport(0, 0, width, height));
		glCall(glScissor(0, 0, width, height));
	}

	void FramebufferOpenGL::BindAsReadBuffer(uint32_t texture)
	{
		glCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO));
		glCall(glReadBuffer(GL_COLOR_ATTACHMENT0 + texture));
	}

	void FramebufferOpenGL::Unbind()
	{
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void FramebufferOpenGL::Recreate()
	{
		DeleteTextures();

		glCall(glGenFramebuffers(1, &m_FBO));
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));

		CreateTextures();
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	const FramebufferSpecification FramebufferOpenGL::GetFramebufferSpecification()
	{
		return m_Specification;
	}

	void FramebufferOpenGL::SetFramebufferSpecification(const FramebufferSpecification &spec)
	{
		m_Specification = spec;
		Recreate();
	}

	Ref<Texture2D> FramebufferOpenGL::GetColorTexture(uint32_t index)
	{
		return m_ColorAttachments.at(index);
	}

	Ref<Texture2D> FramebufferOpenGL::GetDepthTexture()
	{
		return m_DepthAttachment;
	}

	void FramebufferOpenGL::CreateTextures()
	{
		m_ColorAttachments.clear();

		for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
		{
			const auto &colorAttachmentSpec = m_Specification.ColorAttachmentSpecification.Attachments[i];

			if (colorAttachmentSpec.TextureFormat == PixelFormat::None)
			{
				NX_ASSERT(0, "Pixel format cannot be PixelFormat::None for a color attachment");
			}

			Nexus::Graphics::Texture2DSpecification spec;
			spec.Width	 = m_Specification.Width;
			spec.Height	 = m_Specification.Height;
			spec.Format	 = colorAttachmentSpec.TextureFormat;
			spec.Samples = m_Specification.Samples;
			spec.Usage	 = {TextureUsage::Sampled, TextureUsage::RenderTarget};
			auto texture = std::dynamic_pointer_cast<Texture2DOpenGL>(m_Device->CreateTexture2D(spec));
			m_ColorAttachments.push_back(texture);

			glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetHandle(), 0));
		}

		if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::None)
		{
			Nexus::Graphics::Texture2DSpecification spec;
			spec.Width		  = m_Specification.Width;
			spec.Height		  = m_Specification.Height;
			spec.Format		  = m_Specification.DepthAttachmentSpecification.DepthFormat;
			spec.Samples	  = m_Specification.Samples;
			spec.Usage		  = {TextureUsage::DepthStencil};
			m_DepthAttachment = m_Device->CreateTexture2D(spec);

			auto glTexture = std::dynamic_pointer_cast<Texture2DOpenGL>(m_DepthAttachment);

			glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, glTexture->GetHandle(), 0));
		}
	}

	void FramebufferOpenGL::DeleteTextures()
	{
	}
}	 // namespace Nexus::Graphics

#endif