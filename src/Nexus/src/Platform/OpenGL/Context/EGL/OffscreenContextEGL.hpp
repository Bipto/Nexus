#pragma once

#if defined(NX_PLATFORM_EGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/Context/IOffscreenContext.hpp"
	#include "Platform/OpenGL/ContextSpecification.hpp"

	#include "glad/glad_egl.h"

	#include "Platform/X11/X11Include.hpp"

namespace Nexus::GL
{
	class OffscreenContextEGL : public IOffscreenContext
	{
	  public:
		OffscreenContextEGL(EGLDisplay display, const ContextSpecification &spec);
		virtual ~OffscreenContextEGL();
		virtual bool MakeCurrent() override;

		EGLContext GetContext();

	  private:
		EGLDisplay m_EGLDisplay = {};
		EGLContext m_Context	= {};

		ContextSpecification m_Specification;
	};
}	 // namespace Nexus::GL

#endif