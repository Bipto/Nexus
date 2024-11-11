#if defined(NX_PLATFORM_WEBGL)

	#include "OffscreenContextWebGL.hpp"

namespace Nexus::GL
{
	EM_JS(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE, create_offscreen_context, (int width, int height), {
		var newCanvas	 = document.createElement('canvas');
		newCanvas.width	 = width;
		newCanvas.height = height;
		newCanvas.id	 = 'offscreenCanvas';

		document.body.appendChild(newCanvas);

		var gl = newCanvas.getContext('webgl2');

		return GL.registerContext(gl, {majorVersion: 2, minorVersion: 0});
	});

	OffscreenContextWebGL::OffscreenContextWebGL()
	{
		m_Context = create_offscreen_context(1, 1);
		MakeCurrent();
	}

	OffscreenContextWebGL::~OffscreenContextWebGL()
	{
	}

	bool OffscreenContextWebGL::MakeCurrent()
	{
		return emscripten_webgl_make_context_current(m_Context);
	}
}	 // namespace Nexus::GL

#endif