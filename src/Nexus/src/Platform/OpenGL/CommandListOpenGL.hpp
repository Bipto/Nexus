#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/nxpch.hpp"

#include "GL.hpp"

namespace Nexus::Graphics
{
    class CommandListOpenGL : public CommandList
    {
    public:
        CommandListOpenGL() {};
        virtual ~CommandListOpenGL() {};
    };
}

#endif