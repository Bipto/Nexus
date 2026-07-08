#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    class CommandListOpenGL : public ICommandList
    {
      public:
        CommandListOpenGL(const CommandListDescription &spec) : ICommandList(spec)
        {
        }

        virtual ~CommandListOpenGL()
        {
        }
    };
} // namespace Nexus::Graphics

#endif