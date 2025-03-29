#pragma once

#if defined(NX_PLATFORM_EGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"
	#include "Platform/OpenGL/ContextSpecification.hpp"

	#include "glad/glad_egl.h"

	#if defined(NX_PLATFORM_LINUX)
		#include "Platform/X11/X11Include.hpp"
	#endif
namespace Nexus::GL
{
	class OffscreenContextEGL : public IOffscreenContext
	{
	  public:
		OffscreenContextEGL(EGLDisplay display, const ContextSpecification &spec);
		virtual ~OffscreenContextEGL();
		virtual bool MakeCurrent() override;
		virtual bool Validate() override;

		EGLContext GetContext();

	  private:
		EGLDisplay m_EGLDisplay = {};
		EGLContext m_Context	= {};

		ContextSpecification m_Specification;
	};
}	 // namespace Nexus::GL

#endif