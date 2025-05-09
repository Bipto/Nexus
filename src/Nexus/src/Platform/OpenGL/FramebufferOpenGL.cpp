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

	void FramebufferOpenGL::BindAsReadBuffer(uint32_t texture)
	{
		glCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO));
		glCall(glReadBuffer(GL_COLOR_ATTACHMENT0 + texture));
	}

	void FramebufferOpenGL::BindAsDrawBuffer()
	{
		glCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO));
		auto width	= m_Specification.Width;
		auto height = m_Specification.Height;

		std::vector<GLenum> drawBuffers;
		for (size_t i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
		{
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		glDrawBuffers(drawBuffers.size(), drawBuffers.data());

		glCall(glViewport(0, 0, width, height));
		glCall(glScissor(0, 0, width, height));
	}

	void FramebufferOpenGL::Unbind()
	{
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	int32_t FramebufferOpenGL::GetHandle()
	{
		return m_FBO;
	}

	void FramebufferOpenGL::Recreate()
	{
		DeleteTextures();

		glCall(glGenFramebuffers(1, &m_FBO));
		glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));

		CreateTextures();

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Failed to create framebuffer" << std::endl;
		}

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

	Ref<Texture> FramebufferOpenGL::GetColorTexture(uint32_t index)
	{
		return m_ColorAttachments.at(index);
	}

	Ref<Texture> FramebufferOpenGL::GetDepthTexture()
	{
		return m_DepthAttachment;
	}

	void FramebufferOpenGL::CreateTextures()
	{
		m_ColorAttachments.clear();

		for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
		{
			const auto &colorAttachmentSpec = m_Specification.ColorAttachmentSpecification.Attachments[i];

			if (colorAttachmentSpec.TextureFormat == PixelFormat::Invalid)
			{
				NX_ASSERT(0, "Pixel format cannot be PixelFormat::None for a color attachment");
			}

			Graphics::TextureSpecification textureSpec = {};
			textureSpec.Width						   = m_Specification.Width;
			textureSpec.Height						   = m_Specification.Height;
			textureSpec.Format						   = colorAttachmentSpec.TextureFormat;
			textureSpec.Samples						   = m_Specification.Samples;
			textureSpec.Usage						   = Graphics::TextureUsage_Sampled | Graphics::TextureUsage_RenderTarget;

			Ref<Texture>	   texture	 = Ref<Texture>(m_Device->CreateTexture(textureSpec));
			Ref<TextureOpenGL> textureGL = std::dynamic_pointer_cast<TextureOpenGL>(texture);
			m_ColorAttachments.push_back(textureGL);

			GL::AttachTexture(m_FBO, textureGL.get(), 0, 0, Graphics::ImageAspect::Colour, i);
		}

		if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid)
		{
			Graphics::TextureSpecification textureSpec = {};
			textureSpec.Width						   = m_Specification.Width;
			textureSpec.Height						   = m_Specification.Height;
			textureSpec.Format						   = m_Specification.DepthAttachmentSpecification.DepthFormat;
			textureSpec.Samples						   = m_Specification.Samples;
			textureSpec.Usage						   = Graphics::TextureUsage_DepthStencil;
			m_DepthAttachment						   = Ref<Texture>(m_Device->CreateTexture(textureSpec));

			Ref<TextureOpenGL> textureGL = std::dynamic_pointer_cast<TextureOpenGL>(m_DepthAttachment);
			GL::AttachTexture(m_FBO, textureGL.get(), 0, 0, Graphics::ImageAspect::DepthStencil, 0);
		}
	}

	void FramebufferOpenGL::DeleteTextures()
	{
	}
}	 // namespace Nexus::Graphics

#endif