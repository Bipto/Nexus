#pragma once

#if defined(NX_PLATFORM_WGL)

	#include "Nexus-Core/nxpch.hpp"

	#include "Platform/OpenGL/Context/IViewContext.hpp"
	#include "OffscreenContextWGL.hpp"

	#include "Platform/Windows/WindowsInclude.hpp"

namespace Nexus::GL
{
	class ViewContextWGL : public IViewContext
	{
	  public:
		ViewContextWGL(HWND hwnd, HDC hdc, OffscreenContextWGL *pbuffer, const ContextSpecification &spec);
		virtual ~ViewContextWGL();
		virtual bool						MakeCurrent() override;
		virtual void						Swap() override;
		virtual void						SetVSync(bool enabled) override;
		virtual const ContextSpecification &GetSpecification() const override;
		virtual bool						Validate() override;
		virtual const GladGLContext		   &GetContext() const override;

	  private:
		HGLRC CreateSharedContext(HDC hdc, HGLRC sharedContext, const ContextSpecification &spec);

	  private:
		HWND  m_HWND  = {};
		HDC	  m_HDC	  = {};
		HGLRC m_HGLRC = {};

		OffscreenContextWGL *m_PBuffer		 = {};
		ContextSpecification m_Specification = {};

		OpenGLFunctionContext m_FunctionContext = {};
	};
}	 // namespace Nexus::GL

#endif