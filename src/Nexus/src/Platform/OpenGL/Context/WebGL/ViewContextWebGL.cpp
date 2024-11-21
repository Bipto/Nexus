#if defined(NX_PLATFORM_WEBGL)

	#include "ViewContextWebGL.hpp"
	#include "OffscreenContextWebGL.hpp"

	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
	#include <emscripten/val.h>

	#include "Nexus-Core/Timings/Profiler.hpp"

namespace Nexus::GL
{
	EM_JS(void, copy_webgl_canvas_to_2d_canvas, (const char *webglCanvasId, const char *canvas2DId, int x, int y, int width, int height), {
		var sourceCanvas = document.getElementById(UTF8ToString(webglCanvasId));
		var sourceGL	 = sourceCanvas.getContext('webgl2');

		var destCanvas = document.getElementById(UTF8ToString(canvas2DId));
		var destCtx	   = destCanvas.getContext('2d');

		destCtx.drawImage(sourceCanvas, 0, 0);
	});

	std::vector<std::string> GetCanvasIds()
	{
		std::vector<std::string> ids;

		emscripten::val document = emscripten::val::global("document");
		emscripten::val canvases = document.call<emscripten::val, std::string>("getElementsByTagName", std::string("canvas"));

		int length = canvases["length"].as<int>();

		for (int i = 0; i < length; i++)
		{
			emscripten::val canvas = canvases[i];
			std::string		id	   = canvas["id"].as<std::string>();
			ids.push_back(id);
		}

		return ids;
	}

	void PrintCanvasIds()
	{
		std::vector<std::string> canvasIds = GetCanvasIds();
		std::cout << "Canvas IDs:" << std::endl;
		for (const auto &id : canvasIds) { std::cout << id << std::endl; }
	}

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

		OffscreenContextWebGL *offscreenContext = (OffscreenContextWebGL *)m_Device->GetOffscreenContext();
		offscreenContext->Resize(width, height);
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
		NX_PROFILE_FUNCTION();

		OffscreenContextWebGL *offscreenContext	   = (OffscreenContextWebGL *)m_Device->GetOffscreenContext();
		std::string			   offscreenCanvasName = offscreenContext->GetCanvasName();

		uint32_t textureWidth  = m_Framebuffer->GetFramebufferSpecification().Width;
		uint32_t textureHeight = m_Framebuffer->GetFramebufferSpecification().Height;

		{
			NX_PROFILE_SCOPE("Binding framebuffers");
			Ref<Graphics::FramebufferOpenGL> framebufferOpenGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferOpenGL->GetHandle());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		{
			NX_PROFILE_SCOPE("Blit framebuffer");
			glBlitFramebuffer(0, 0, textureWidth, textureHeight, 0, 0, textureWidth, textureHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		{
			NX_PROFILE_SCOPE("Commit frame");
			emscripten_webgl_commit_frame();
		}

		{
			NX_PROFILE_SCOPE("Copy to 2D canvas");
			copy_webgl_canvas_to_2d_canvas(offscreenCanvasName.c_str(), m_CanvasName.c_str(), 0, 0, textureWidth, textureHeight);
		}
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