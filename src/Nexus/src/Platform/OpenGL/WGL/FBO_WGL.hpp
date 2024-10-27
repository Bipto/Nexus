#pragma once

#if defined(NX_PLATFORM_WGL)

	#include "Nexus-Core/nxpch.hpp"

	#include "PBufferWGL.hpp"
	#include "Platform/OpenGL/FBO.hpp"

	#include "Platform/Windows/WindowsInclude.hpp"

namespace Nexus::GL
{
	class FBO_WGL : public FBO
	{
	  public:
		FBO_WGL(HWND hwnd, PBufferWGL *pbuffer, const ContextSpecification &spec);
		virtual ~FBO_WGL();
		virtual bool						MakeCurrent() override;
		virtual void						Swap() override;
		virtual void						SetVSync(bool enabled) override;
		virtual const ContextSpecification &GetSpecification() const override;

	  private:
		HGLRC CreateSharedContext(HDC hdc, HGLRC sharedContext, const ContextSpecification &spec);

	  private:
		HWND  m_HWND  = {};
		HDC	  m_HDC	  = {};
		HGLRC m_HGLRC = {};

		PBufferWGL			*m_PBuffer		 = {};
		ContextSpecification m_Specification = {};
	};
}	 // namespace Nexus::GL

#endif