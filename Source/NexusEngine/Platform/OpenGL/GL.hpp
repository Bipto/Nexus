#pragma once

#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#include <GLES3/gl32.h>
#else
#include "glad/glad.h"
#endif

namespace Nexus::GL
{
    void ClearErrors();
    bool CheckErrors();
    std::string GetErrorMessageFromCode(const GLenum error);
}