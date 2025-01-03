#if defined(NX_PLATFORM_WEBGL)

	#include "OffscreenContextWebGL.hpp"

namespace Nexus::GL
{
	EM_JS(void, create_offscreen_context, (const char *id), {
		var canvasId				  = UTF8ToString(id);
		var newCanvas				  = document.createElement('canvas');
		newCanvas.id				  = canvasId;
		newCanvas.style.left		  = '0px';
		newCanvas.style.top			  = '0px';
		newCanvas.style.zIndex		  = '1000';
		newCanvas.style.position	  = 'fixed';
		newCanvas.style.zIndex		  = '1000';
		newCanvas.style.display		  = 'inline';
		newCanvas.style.pointerEvents = 'none';

		// document.body.appendChild(newCanvas);

		newCanvas.width		   = window.innerWidth * window.devicePixelRatio;
		newCanvas.height	   = window.innerHeight * window.devicePixelRatio;
		newCanvas.style.width  = window.innerWidth + 'px';
		newCanvas.style.height = window.innerHeight + 'px';

		document.body.prepend(newCanvas);

		window.addEventListener(
			'resize',
			function(event) {
				var element = document.getElementById(UTF8ToString(id));

				// style.width/height resizes the physical control, .width/height resizes the OpenGL virtual canvas
				//.width/height need to be scaled to physical pixels while style.width/height needs to be in CSS pixel units
				element.width		 = window.innerWidth * window.devicePixelRatio;
				element.height		 = window.innerHeight * window.devicePixelRatio;
				element.style.width	 = window.innerWidth + 'px';
				element.style.height = window.innerHeight + 'px';
			},
			true);
	});

	OffscreenContextWebGL::OffscreenContextWebGL(const std::string &canvasName) : m_CanvasName(canvasName)
	{
		m_CSS_Selector = std::string("#") + m_CanvasName;

		create_offscreen_context(m_CanvasName.c_str());

		EmscriptenWebGLContextAttributes attr = {};
		emscripten_webgl_init_context_attributes(&attr);
		attr.antialias	  = EM_FALSE;
		attr.majorVersion = 2;
		attr.minorVersion = 0;

		std::string cssSelector = GetCSS_SelectorString();

		m_Context = emscripten_webgl_create_context(cssSelector.c_str(), &attr);
		if (!m_Context)
		{
			std::cout << "Failed to create context" << std::endl;
		}

		MakeCurrent();
	}

	OffscreenContextWebGL::~OffscreenContextWebGL()
	{
	}

	bool OffscreenContextWebGL::MakeCurrent()
	{
		emscripten_webgl_make_context_current(m_Context);
		return true;
	}

	const std::string &Nexus::GL::OffscreenContextWebGL::GetCanvasName()
	{
		return m_CanvasName;
	}

	const std::string &Nexus::GL::OffscreenContextWebGL::GetCSS_SelectorString()
	{
		return m_CSS_Selector;
	}

	void Nexus::GL::OffscreenContextWebGL::Resize()
	{
		emscripten::val document = emscripten::val::global("document");
		emscripten::val element	 = document.call<emscripten::val>("getElementById", m_CanvasName);

		emscripten::val window = emscripten::val::global("window");

		if (element.isUndefined() || element.isNull())
		{
			std::cout << "Could not find element" << std::endl;
			return;
		}

		emscripten::val style = element["style"];

		int windowWidth	 = window["innerWidth"].as<int>();
		int windowHeight = window["innerHeight"].as<int>();

		style.set("width", windowWidth);
		style.set("height", windowHeight);
	}

	bool Nexus::GL::OffscreenContextWebGL::Validate()
	{
		return m_Context != 0;
	}

}	 // namespace Nexus::GL

#endif
