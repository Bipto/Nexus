#pragma once

#if defined(NX_PLATFORM_EGL)

	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/OpenGL/PBuffer.hpp"

	#include "glad/glad_egl.h"

namespace Nexus::GL
{
	class PBufferEGL : public PBuffer
	{
	  public:
		PBufferEGL();
		virtual ~PBufferEGL();
		virtual bool MakeCurrent() override;

	  private:
		EGLDisplay m_Display = {};
		EGLSurface m_Surface = {};
		EGLContext m_Context = {};
	};
}	 // namespace Nexus::GL

#endif