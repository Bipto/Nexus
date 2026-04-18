#if defined(NX_PLATFORM_OPENGL)

	#include "FramebufferOpenGL.hpp"

	#include "GraphicsDeviceOpenGL.hpp"
	#include "TextureOpenGL.hpp"

namespace Nexus::Graphics
{
	FramebufferOpenGL::FramebufferOpenGL(const FramebufferTextureSetDescription &desc, GraphicsDeviceOpenGL *graphicsDevice)
		: m_Description(desc),
		  m_Device(graphicsDevice)
	{
		Create();
	}

	FramebufferOpenGL::~FramebufferOpenGL()
	{
		GL::ExecuteGLCommands([&](const GladGLContext &context) { context.DeleteFramebuffers(1, &m_FBO); });
	}

	const FramebufferTextureSetDescription FramebufferOpenGL::GetTextureSetDescription() const
	{
		return m_Description;
	}

	void FramebufferOpenGL::BindAsReadBuffer(uint32_t texture, const GladGLContext &context)
	{
		glCall(context.BindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO));
		glCall(context.ReadBuffer(GL_COLOR_ATTACHMENT0 + texture));
	}

	void FramebufferOpenGL::BindAsDrawBuffer(const GladGLContext &context)
	{
		glCall(context.BindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO));
		auto [width, height] = m_Description.GetSize();

		std::vector<GLenum> drawBuffers;
		for (size_t i = 0; i < m_Description.ColourAttachments.size(); i++) { drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i); }

		context.DrawBuffers(drawBuffers.size(), drawBuffers.data());

		glCall(context.Viewport(0, 0, width, height));
		glCall(context.Scissor(0, 0, width, height));
	}

	void FramebufferOpenGL::Unbind()
	{
		GL::IOffscreenContext *offscreenContext = m_Device->GetOffscreenContext();
		GL::ExecuteGLCommands([&](const GladGLContext &context) { glCall(context.BindFramebuffer(GL_FRAMEBUFFER, 0)); });
	}

	int32_t FramebufferOpenGL::GetHandle()
	{
		return m_FBO;
	}

	void FramebufferOpenGL::Create()
	{
		GL::IOffscreenContext *offscreenContext = m_Device->GetOffscreenContext();
		GL::ExecuteGLCommands(
			[&](const GladGLContext &context)
			{
				// create the framebuffer
				glCall(context.GenFramebuffers(1, &m_FBO));
				glCall(context.BindFramebuffer(GL_FRAMEBUFFER, m_FBO));

				// attach colour targets
				for (size_t i = 0; i < m_Description.ColourAttachments.size(); i++)
				{
					const Graphics::FramebufferColourAttachmentDescription &colourAttachment = m_Description.ColourAttachments.at(i);
					TextureHandle											texture			 = colourAttachment.ColourAttachment.TargetTexture;
					GL::AttachTexture(m_FBO, colourAttachment.ColourAttachment, texture->IsDepth(), i, context);
				}

				// attach depth target if needed
				if (m_Description.DepthAttachment.has_value())
				{
					Graphics::FramebufferTextureDescription depthAttachment = m_Description.DepthAttachment.value();
					TextureHandle							texture			= depthAttachment.TargetTexture;
					GL::AttachTexture(m_FBO, depthAttachment, texture->IsDepth(), 0, context);
				}

				// validate the framebuffer
				GLenum status = context.CheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					std::cout << "Failed to create framebuffer" << std::endl;
				}

				glCall(context.BindFramebuffer(GL_FRAMEBUFFER, 0));
			});
	}
}	 // namespace Nexus::Graphics

#endif