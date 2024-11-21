#if defined(NX_PLATFORM_WEBGL)

	#include "OffscreenContextWebGL.hpp"

namespace Nexus::GL
{
	EM_JS(void, create_offscreen_context, (const char *id, int width, int height), {
		var newCanvas	 = document.createElement('canvas');
		newCanvas.width	 = width;
		newCanvas.height = height;
		newCanvas.id			= UTF8ToString(id);
		newCanvas.style.display = 'none';

		document.body.appendChild(newCanvas);
	});

	EM_JS(void, resize_canvas_element, (const char *canvasId, int width, int height), {
		var canvas	  = document.getElementById(UTF8ToString(canvasId));
		canvas.width  = width;
		canvas.height = height;
	});

	OffscreenContextWebGL::OffscreenContextWebGL(const std::string &canvasName) : m_CanvasName(canvasName)
	{
		m_CSS_Selector = std::string("#") + m_CanvasName;

		create_offscreen_context(m_CanvasName.c_str(), m_Width, m_Height);

		EmscriptenWebGLContextAttributes attr = {};
		emscripten_webgl_init_context_attributes(&attr);
		attr.antialias			   = EM_FALSE;
		attr.majorVersion		   = 2;
		attr.minorVersion		   = 0;
		attr.preserveDrawingBuffer = EM_FALSE;
		attr.antialias			   = EM_FALSE;
		attr.powerPreference	   = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
		// attr.renderViaOffscreenBackBuffer = EM_TRUE;

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
		return emscripten_webgl_make_context_current(m_Context);
	}

	const std::string &Nexus::GL::OffscreenContextWebGL::GetCanvasName()
	{
		return m_CanvasName;
	}

	const std::string &Nexus::GL::OffscreenContextWebGL::GetCSS_SelectorString()
	{
		return m_CSS_Selector;
	}

	void Nexus::GL::OffscreenContextWebGL::Resize(uint32_t width, uint32_t height)
	{
		if (width > m_Width)
		{
			m_Width = width;
		}

		if (height > m_Height)
		{
			m_Height = height;
		}

		resize_canvas_element(m_CanvasName.c_str(), (int)m_Width, (int)m_Height);
	}
}	 // namespace Nexus::GL

#endif