#if defined(NX_PLATFORM_WEBGL)

	#include "ViewContextWebGL.hpp"

namespace Nexus::GL
{
	EM_JS(void, transfer_pixels_to_canvas, (const char *canvasId, const std::byte *pixels, int width, int height), {
		var canvas2d = document.getElementById(UTF8ToString(canvasId));
		var ctx		 = canvas2d.getContext('2d');

		var imageData = ctx.createImageData(width, height);
		imageData.data.set(HEAPU8.subarray(pixels, pixels + width * height * 4));
		ctx.putImageData(imageData, 0, 0);
	});

	ViewContextWebGL::ViewContextWebGL(const std::string					 &canvasName,
									   Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice,
									   const ContextSpecification			 &spec)
		: m_Device(graphicsDevice),
		  m_Specification(spec),
		  m_CanvasName(canvasName)
	{
		int width, height;
		emscripten_get_canvas_element_size(m_CanvasName.c_str(), &width, &height);

		Graphics::FramebufferSpecification framebufferSpec		 = {};
		framebufferSpec.Width									 = (uint32_t)width;
		framebufferSpec.Height									 = (uint32_t)height;
		framebufferSpec.ColorAttachmentSpecification.Attachments = {Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
		framebufferSpec.DepthAttachmentSpecification			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;
		framebufferSpec.Samples									 = Graphics::SampleCount::SampleCount1;
		m_Framebuffer											 = graphicsDevice->CreateFramebuffer(framebufferSpec);
	}

	ViewContextWebGL::~ViewContextWebGL()
	{
	}

	bool Nexus::GL::ViewContextWebGL::MakeCurrent()
	{
		IOffscreenContext *context = m_Device->GetOffscreenContext();
		context->MakeCurrent();
		return true;
	}

	void ViewContextWebGL::BindAsRenderTarget()
	{
		Ref<Graphics::FramebufferOpenGL> framebufferGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);
		if (framebufferGL)
		{
			framebufferGL->BindAsRenderTarget();
		}
	}

	void ViewContextWebGL::BindAsDrawTarget()
	{
		Ref<Graphics::FramebufferOpenGL> framebufferGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);
		if (framebufferGL)
		{
			framebufferGL->BindAsDrawBuffer();
		}
	}

	void Nexus::GL::ViewContextWebGL::Swap()
	{
		uint32_t				 textureWidth  = m_Framebuffer->GetFramebufferSpecification().Width;
		uint32_t				 textureHeight = m_Framebuffer->GetFramebufferSpecification().Height;
		Ref<Graphics::Texture2D> texture	   = m_Framebuffer->GetColorTexture(0);
		texture->GetData(m_Pixels, 0, 0, 0, textureWidth, textureHeight);

		transfer_pixels_to_canvas(m_CanvasName.c_str(), m_Pixels.data(), (int)textureWidth, (int)textureHeight);
	}

	void Nexus::GL::ViewContextWebGL::SetVSync(bool enabled)
	{
	}

	const ContextSpecification &Nexus::GL::ViewContextWebGL::GetSpecification() const
	{
		return m_Specification;
	}

}	 // namespace Nexus::GL

#endif