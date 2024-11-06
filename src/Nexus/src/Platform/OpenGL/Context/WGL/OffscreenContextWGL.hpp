#pragma once

#if defined(NX_PLATFORM_WGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"

	#include "Platform/Windows/WindowsInclude.hpp"
	#include "glad/glad_wgl.h"

namespace Nexus::GL
{
	class OffscreenContextWGL : public IOffscreenContext
	{
	  public:
		OffscreenContextWGL();
		virtual ~OffscreenContextWGL();
		virtual bool MakeCurrent() override;

		HGLRC GetHGLRC();

	  private:
		inline static void					LoadGLFunctionsIfNeeded(HDC hdc);
		std::tuple<HWND, HGLRC, HDC>		CreateTemporaryWindow();
		std::tuple<HPBUFFERARB, HDC, HGLRC> CreatePBufferContext(HDC hdc);

	  private:
		HGLRC		m_HGLRC	  = {};
		HPBUFFERARB m_PBuffer = {};
		HDC			m_HDC	  = {};

	  private:
		inline static bool s_GLFunctionsLoaded = false;
	};
}	 // namespace Nexus::GL

#endif