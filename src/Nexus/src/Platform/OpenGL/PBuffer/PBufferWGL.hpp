#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "PBuffer.hpp"

#include "Platform/Windows/WindowsInclude.hpp"
#include "glad/glad_wgl.h"

namespace Nexus::GL
{
	class PBufferWGL : public PBuffer
	{
	  public:
		PBufferWGL();
		virtual ~PBufferWGL();
		virtual void MakeCurrent() override;

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