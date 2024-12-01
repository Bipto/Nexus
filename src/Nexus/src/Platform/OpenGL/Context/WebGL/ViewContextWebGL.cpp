#define NX_PLATFORM_WEBGL
#if defined(NX_PLATFORM_WEBGL)

	#include "ViewContextWebGL.hpp"
	#include "OffscreenContextWebGL.hpp"

	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
	#include <emscripten/val.h>

	#include "Nexus-Core/Timings/Profiler.hpp"

	#include "Nexus-Core/Point.hpp"

	#include "Platform/OpenGL/TextureOpenGL.hpp"

const std::string c_VertexShader = R"(
	attribute vec3 a_Position;
	attribute vec2 a_TexCoord;
	varying vec2 v_TexCoord;

	void main()
	{
		gl_Position = vec4(a_Position, 1.0);
		v_TexCoord = a_TexCoord;
	}
	)";

const std::string c_FragmentShader = R"(
	precision mediump float;
	
	uniform sampler2D u_Texture;
	varying vec2 v_TexCoord;

	void main()
	{
		gl_FragColor = texture2D(u_Texture, v_TexCoord);
	}
)";

namespace Nexus::GL
{
	EM_JS(void, copy_webgl_canvas_to_2d_canvas, (const char *webglCanvasId, const char *canvas2DId, int x, int y, int width, int height), {
		var sourceCanvas = document.getElementById(UTF8ToString(webglCanvasId));
		var sourceGL	 = sourceCanvas.getContext('webgl2');

		var destCanvas = document.getElementById(UTF8ToString(canvas2DId));
		var destCtx	   = destCanvas.getContext('2d');

		destCtx.drawImage(sourceCanvas, 0, 0);
	});

	EM_JS(void, copy_webgl_canvas_to_bitmap_renderer, (const char *webglCanvasId, const char *canvas2DId, int x, int y, int width, int height), {
		var sourceCanvas = document.getElementById(UTF8ToString(webglCanvasId));
		var sourceGL	 = sourceCanvas.getContext('webgl2');

		var destCanvas = document.getElementById(UTF8ToString(canvas2DId));
		var destCtx	   = destCanvas.getContext('bitmaprenderer');

		// destCtx.drawImage(sourceCanvas, 0, 0);

		const imageBitmap = createImageBitmap(sourceCanvas);
		destCtx.transferFromImageBitmap(imageBitmap);
	});

	BoundingClientRect GetBoundingClientRect(const std::string &id)
	{
		emscripten::val document = emscripten::val::global("document");
		emscripten::val element	 = document.call<emscripten::val>("getElementById", id);
		emscripten::val window	 = emscripten::val::global("window");

		if (element.isUndefined() || element.isNull())
		{
			std::cout << "Could not find element" << std::endl;
			return {};
		}
		emscripten::val rect = element.call<emscripten::val>("getBoundingClientRect");

		if (rect.isUndefined() || rect.isNull())
		{
			std::cout << "Could not get bounding client rect from element" << std::endl;
			return {};
		}

		double pixelRatio = window["devicePixelRatio"].as<double>();
		double left		  = rect["x"].as<double>();
		double top		  = rect["y"].as<double>();
		double width	  = rect["width"].as<double>();
		double height	  = rect["height"].as<double>();

		return BoundingClientRect {.Left = left * pixelRatio, .Top = top * pixelRatio, .Width = width * pixelRatio, .Height = height * pixelRatio};
	}

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

	EM_JS(void, attach_resize_handler, (const char *id), {
		window.addEventListener(
			'resize',
			function(event) {
				var canvas	  = document.getElementById(UTF8ToString(id));
				canvas.width  = canvas.style.width * window.devicePixelRatio;
				canvas.height = canvas.style.height * window.devicePixelRatio;
			},
			true);
	});

	EM_JS(int, get_element_width, (const char *id), {
		var element = document.getElementById(UTF8ToString(id));
		return element.width;
	});

	EM_JS(int, get_element_height, (const char *id), {
		var element = document.getElementById(UTF8ToString(id));
		return element.height;
	});

	ViewContextWebGL::ViewContextWebGL(const std::string					 &canvasName,
									   Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice,
									   const ContextSpecification			 &spec)
		: m_Device(graphicsDevice),
		  m_Specification(spec),
		  m_CanvasName(canvasName)
	{
		CreateFramebuffer();
		attach_resize_handler(m_CanvasName.c_str());
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
		offscreenContext->Resize();

		uint32_t textureWidth  = m_Framebuffer->GetFramebufferSpecification().Width;
		uint32_t textureHeight = m_Framebuffer->GetFramebufferSpecification().Height;

		Ref<Graphics::FramebufferOpenGL> framebufferOpenGL = std::dynamic_pointer_cast<Graphics::FramebufferOpenGL>(m_Framebuffer);
		{
			NX_PROFILE_SCOPE("Binding framebuffers");

			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferOpenGL->GetHandle());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		{
			NX_PROFILE_SCOPE("Blit framebuffer");

			BoundingClientRect fullscreenRect = GetBoundingClientRect(offscreenCanvasName);
			BoundingClientRect viewRect		  = GetBoundingClientRect(m_CanvasName);

			std::chrono::steady_clock::time_point tickTime = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::nanoseconds>(tickTime - m_Start).count() > 2)
			{
				if (viewRect.Width != m_BoundingClientRect.Width || viewRect.Height != m_BoundingClientRect.Height)
				{
					CreateFramebuffer();
					m_BoundingClientRect = viewRect;
				}
				m_Start = std::chrono::steady_clock::now();
			}

			float x		 = viewRect.Left;
			float y		 = fullscreenRect.Height - (viewRect.Top + viewRect.Height);
			float width	 = viewRect.Width;
			float height = viewRect.Height;

			glEnable(GL_SCISSOR_TEST);
			glViewport(x, y, width, height);
			glScissor(x, y, width, height);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glBlitFramebuffer(0, 0, textureWidth, textureHeight, x, y, x + width, y + height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
	}

	void Nexus::GL::ViewContextWebGL::SetVSync(bool enabled)
	{
	}

	const ContextSpecification &Nexus::GL::ViewContextWebGL::GetSpecification() const
	{
		return m_Specification;
	}

	void Nexus::GL::ViewContextWebGL::HandleResize()
	{
		CreateFramebuffer();
	}

	void Nexus::GL::ViewContextWebGL::CreateFramebuffer()
	{
		BoundingClientRect rect = GetBoundingClientRect(m_CanvasName);

		Graphics::FramebufferSpecification framebufferSpec		 = {};
		framebufferSpec.Width									 = (uint32_t)rect.Width;
		framebufferSpec.Height									 = (uint32_t)rect.Height;
		framebufferSpec.ColorAttachmentSpecification.Attachments = {Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
		framebufferSpec.DepthAttachmentSpecification			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;
		framebufferSpec.Samples									 = Graphics::SampleCount::SampleCount1;
		m_Framebuffer											 = m_Device->CreateFramebuffer(framebufferSpec);
	}

}	 // namespace Nexus::GL

#endif