#pragma once

#include "Platform/OpenGL/Context/IOffscreenContext.hpp"
#include "Platform/OpenGL/ContextDescription.hpp"

#include "egl_include.hpp"
#include "glad/gl.h"

#if defined(NX_PLATFORM_LINUX)
#include "Platform/X11/X11Include.hpp"
#endif

namespace Nexus::GL
{
    class OffscreenContextEGL final : public IOffscreenContext
    {
      public:
        OffscreenContextEGL(const ContextDescription &spec, EGLDisplay display);
        virtual ~OffscreenContextEGL();
        bool MakeCurrent() final;
        bool Validate() final;

        EGLContext GetEGLContext();

      private:
        EGLDisplay m_EGLDisplay = {};
        EGLContext m_Context = {};

        ContextDescription m_Description;
        GladGLContext m_GladContext = {};
    };
} // namespace Nexus::GL