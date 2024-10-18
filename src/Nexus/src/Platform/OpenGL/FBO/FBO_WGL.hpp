#pragma once

#include "FBO.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Platform/OpenGL/PBuffer/PBufferWGL.hpp"

#include "Platform/Windows/WindowsInclude.hpp"

namespace Nexus::GL
{
	class FBO_WGL : public FBO
	{
	  public:
		FBO_WGL(HWND hwnd, PBufferWGL *pbuffer);
		virtual ~FBO_WGL();
		virtual void MakeCurrent() override;
		virtual void Swap() override;
		virtual void SetVSync(bool enabled) override;

	  private:
		HGLRC CreateSharedContext(HDC hdc, HGLRC sharedContext);

	  private:
		HWND  m_HWND  = {};
		HDC	  m_HDC	  = {};
		HGLRC m_HGLRC = {};

		PBufferWGL *m_PBuffer = {};
	};
}	 // namespace Nexus::GL