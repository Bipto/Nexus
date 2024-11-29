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

	struct BoundingClientRect
	{
		double Left	  = 0;
		double Top	  = 0;
		double Width  = 0;
		double Height = 0;
	};

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

		/* 		emscripten::val ownerDocument = element["ownerDocument"];
				emscripten::val defaultView	  = ownerDocument["defaultView"];

				top += defaultView["pageYOffset"].as<double>(); */

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
		BoundingClientRect rect = GetBoundingClientRect(m_CanvasName);

		int width  = get_element_width(m_CanvasName.c_str());
		int height = get_element_height(m_CanvasName.c_str());

		Graphics::FramebufferSpecification framebufferSpec		 = {};
		framebufferSpec.Width									 = (uint32_t)width;
		framebufferSpec.Height									 = (uint32_t)height;
		framebufferSpec.ColorAttachmentSpecification.Attachments = {Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
		framebufferSpec.DepthAttachmentSpecification			 = Graphics::PixelFormat::D24_UNorm_S8_UInt;
		framebufferSpec.Samples									 = Graphics::SampleCount::SampleCount1;
		m_Framebuffer											 = graphicsDevice->CreateFramebuffer(framebufferSpec);

		m_ShaderHandle = CreateShader();
		m_VBO		   = CreateVBO();
		m_VAO		   = CreateVAO();

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
			// glBlitFramebuffer(0, 0, textureWidth, textureHeight, 0, 0, textureWidth, textureHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			BoundingClientRect fullscreenRect = GetBoundingClientRect(offscreenCanvasName);
			BoundingClientRect viewRect		  = GetBoundingClientRect(m_CanvasName);

			emscripten::val window		 = emscripten::val::global("window");
			double			windowHeight = window["innerHeight"].as<double>();

			float left = viewRect.Left;
			// float bottom = fullscreenRect.Height - (viewRect.Top + viewRect.Height);
			// float viewportY = fullscreenRect.Height - (viewRect.Top + viewRect.Height);
			float viewportY = windowHeight - (viewRect.Top + viewRect.Height);
			// float scissorY	= fullscreenRect.Height - (viewRect.Height - viewRect.Top);
			float scissorY = fullscreenRect.Height - (viewRect.Top + viewRect.Height);
			float width	   = viewRect.Width;
			float height   = viewRect.Height;

			glEnable(GL_SCISSOR_TEST);
			glViewport(left, scissorY, width, height);
			glScissor(left, scissorY, width, height);

			glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			/* glBlitFramebuffer(0,
							  0,
							  viewRect.Width,
							  viewRect.Height,
							  viewRect.Left,
							  viewRect.Top,
							  viewRect.Width,
							  viewRect.Height,
							  GL_COLOR_BUFFER_BIT,
							  GL_NEAREST); */

			/* glUseProgram(m_ShaderHandle);
			Ref<Graphics::Texture2DOpenGL> colourTexture =
				std::dynamic_pointer_cast<Graphics::Texture2DOpenGL>(framebufferOpenGL->GetColorTexture(0));
			colourTexture->Bind(0);

			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0); */

			glBlitFramebuffer(0, 0, textureWidth, textureHeight, left, scissorY, textureWidth, textureHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}

		{
			NX_PROFILE_SCOPE("Commit frame");
			emscripten_webgl_commit_frame();
		}

		{
			NX_PROFILE_SCOPE("Copy to 2D canvas");
			// copy_webgl_canvas_to_2d_canvas(offscreenCanvasName.c_str(), m_CanvasName.c_str(), 0, 0, textureWidth, textureHeight);
			// copyToBitmapContext(offscreenCanvasName.c_str(), m_CanvasName.c_str());
			// copy_webgl_canvas_to_bitmap_renderer(offscreenCanvasName.c_str(), m_CanvasName.c_str(), 0, 0, textureWidth, textureHeight);
		}
	}

	void Nexus::GL::ViewContextWebGL::SetVSync(bool enabled)
	{
	}

	const ContextSpecification &Nexus::GL::ViewContextWebGL::GetSpecification() const
	{
		return m_Specification;
	}

	unsigned int Nexus::GL::ViewContextWebGL::CreateShaderStage(const std::string &shader, GLenum stage)
	{
		unsigned int handle = glCreateShader(stage);
		const char	*source = shader.c_str();
		glShaderSource(handle, 1, &source, nullptr);
		glCompileShader(handle);

		int success;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(handle, 512, NULL, infoLog);
			std::cout << infoLog << std::endl;
		}

		return handle;
	}

	unsigned int Nexus::GL::ViewContextWebGL::CreateShader()
	{
		unsigned int vertexShader	= CreateShaderStage(c_VertexShader, GL_VERTEX_SHADER);
		unsigned int fragmentShader = CreateShaderStage(c_FragmentShader, GL_FRAGMENT_SHADER);

		unsigned int program = glCreateProgram();

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		int success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(program, 512, nullptr, infoLog);
			std::cout << infoLog << std::endl;
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);

		return program;
	}

	unsigned int Nexus::GL::ViewContextWebGL::CreateVBO()
	{
		// layout: vec2 position - vec2 texCoord
		float quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
								-1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	-1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	1.0f, 1.0f};

		unsigned int vbo = 0;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return vbo;
	}

	unsigned int Nexus::GL::ViewContextWebGL::CreateVAO()
	{
		unsigned int vao = 0;
		glGenVertexArrays(1, &vao);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));
		glBindVertexArray(0);

		return vao;
	}

}	 // namespace Nexus::GL

#endif