#pragma once

#if defined(NX_PLATFORM_EGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "OffscreenContextEGL.hpp"
	#include "Platform/OpenGL/Context/IViewContext.hpp"

	#include "glad/egl.h"

	#if defined(NX_PLATFORM_LINUX)
		#include "Platform/X11/X11Include.hpp"
	#endif
namespace Nexus::GL
{
	class ViewContextEGL : public IViewContext
	{
	  public:
		ViewContextEGL(EGLDisplay display, EGLNativeWindowType window, OffscreenContextEGL *pbuffer, const ContextSpecification &spec);
		virtual ~ViewContextEGL();
		virtual bool						MakeCurrent() override;
		virtual void						Swap() override;
		virtual void						SetVSync(bool enabled) override;
		virtual const ContextSpecification &GetSpecification() const override;
		virtual bool						Validate() override;
		virtual const GladGLContext		   &GetContext() const override;

	  private:
		EGLDisplay			m_EGLDisplay   = {};
		EGLNativeWindowType m_NativeWindow = {};
		EGLSurface			m_Surface	   = {};
		EGLContext			m_Context	   = {};

		OffscreenContextEGL *m_PBuffer		 = {};
		ContextSpecification m_Specification = {};

		GladGLContext m_GladContext = {};
	};
}	 // namespace Nexus::GL

#endif