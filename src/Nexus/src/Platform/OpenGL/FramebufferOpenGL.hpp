#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"
	#include "Nexus-Core/Graphics/Framebuffer.hpp"

namespace Nexus::Graphics
{
	// forward declaration
	class GraphicsDeviceOpenGL;

	class FramebufferOpenGL : public Framebuffer
	{
	  public:
		FramebufferOpenGL(const FramebufferSpecification &spec, GraphicsDeviceOpenGL *graphicsDevice);
		~FramebufferOpenGL();

		void BindAsRenderTarget();
		void BindAsReadBuffer(uint32_t texture);
		void Unbind();

		virtual const FramebufferSpecification GetFramebufferSpecification() override;
		virtual void						   SetFramebufferSpecification(const FramebufferSpecification &spec) override;

		virtual Ref<Texture2D> GetColorTexture(uint32_t index = 0) override;
		virtual Ref<Texture2D> GetDepthTexture() override;

	  private:
		virtual void Recreate() override;
		void		 CreateTextures();
		void		 DeleteTextures();

	  private:
		unsigned int m_FBO;

		std::vector<Ref<Texture2D>> m_ColorAttachments;
		Ref<Texture2D>				m_DepthAttachment;

		GraphicsDeviceOpenGL *m_Device = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif