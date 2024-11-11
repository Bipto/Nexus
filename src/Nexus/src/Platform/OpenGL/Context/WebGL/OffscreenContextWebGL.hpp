#pragma once

#if defined(NX_PLATFORM_WEBGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"

	#include <GLES3/gl3.h>

	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
	#include <emscripten/val.h>

namespace Nexus::GL
{
	class OffscreenContextWebGL : public IOffscreenContext
	{
	  public:
		OffscreenContextWebGL();
		virtual ~OffscreenContextWebGL();
		virtual bool MakeCurrent() override;

	  private:
		EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_Context = {};
	};

}	 // namespace Nexus::GL

#endif