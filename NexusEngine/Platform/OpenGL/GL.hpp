#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <gl/GL.h>
#else
#include "glad/glad.h"
#endif

namespace Nexus::GL
{
    void ClearErrors();
    bool CheckErrors();
    std::string GetErrorMessageFromCode(const GLenum error);
}