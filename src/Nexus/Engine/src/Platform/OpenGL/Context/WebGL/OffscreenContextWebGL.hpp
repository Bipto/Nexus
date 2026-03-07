#pragma once

#if defined(NX_PLATFORM_WEBGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"

	#include <GLES3/gl3.h>

	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
	#include <emscripten/val.h>

	#include "Nexus-Core/Point.hpp"

namespace Nexus::GL
{
	class OffscreenContextWebGL : public IOffscreenContext
	{
	  public:
		OffscreenContextWebGL(const std::string &canvasName);
		virtual ~OffscreenContextWebGL();
		virtual bool MakeCurrent() override;
		virtual bool Validate() override;

		const std::string &GetCanvasName();
		const std::string &GetCSS_SelectorString();

	  private:
		std::string m_CanvasName   = {};
		std::string m_CSS_Selector = {};

		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_Context = {};
		uint32_t						m_Width	  = 512;
		uint32_t						m_Height  = 512;
	};

}	 // namespace Nexus::GL

#endif