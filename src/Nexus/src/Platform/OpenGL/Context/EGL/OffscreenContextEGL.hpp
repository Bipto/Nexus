#pragma once

#if defined(NX_PLATFORM_EGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"
	#include "Platform/OpenGL/ContextSpecification.hpp"

	#include "egl_include.hpp"
	#include "glad/gl.h"

	#if defined(NX_PLATFORM_LINUX)
		#include "Platform/X11/X11Include.hpp"
	#endif
namespace Nexus::GL
{
	class OffscreenContextEGL : public IOffscreenContext
	{
	  public:
		OffscreenContextEGL(const ContextSpecification &spec, EGLDisplay display);
		virtual ~OffscreenContextEGL();
		virtual bool				 MakeCurrent() override;
		virtual bool				 Validate() override;
		virtual const GladGLContext &GetContext() const override;

		EGLContext GetEGLContext();

	  private:
		EGLDisplay m_EGLDisplay = {};
		EGLContext m_Context	= {};

		ContextSpecification m_Specification;
		GladGLContext		 m_GladContext = {};
	};
}	 // namespace Nexus::GL

#endif