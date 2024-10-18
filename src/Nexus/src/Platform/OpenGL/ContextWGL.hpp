#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Context.hpp"
#include "Platform/Windows/WindowsInclude.hpp"
#include "glad/glad_wgl.h"

namespace Nexus::GL
{
	class ContextWGL : public Context
	{
	  public:
		ContextWGL(uint32_t width, uint32_t height);
		ContextWGL(HWND hwnd);
		virtual ~ContextWGL();
		virtual void		  Swap() override;
		virtual void		  MakeCurrent() override;
		virtual ContextSource GetSource() override;

	  private:
		void LoadOpenGLIfNeeded();
		void CreatePbufferContext(uint32_t width, uint32_t height);

	  private:
		ContextSource m_ContextSource = {};
		HGLRC		  m_HGLRC		  = {};
		HPBUFFERARB	  m_PBuffer		  = {};
		HDC			  m_PBufferHDC	  = {};

	  private:
		inline static HWND s_HWND			 = nullptr;
		inline static HDC  s_HDC			 = {};
		inline static bool s_FunctionsLoaded = false;
	};
}	 // namespace Nexus::GL