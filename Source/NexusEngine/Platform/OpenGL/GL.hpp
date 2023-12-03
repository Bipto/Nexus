#pragma once

#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#include <GLES3/gl32.h>
#else
#include "glad/glad.h"
#endif

#include "Nexus/Graphics/TextureFormat.hpp"
#include "Nexus/Graphics/DepthFormat.hpp"

namespace Nexus::GL
{
    void ClearErrors();
    bool CheckErrors();
    std::string GetErrorMessageFromCode(const GLenum error);

    GLenum GetColorTextureFormat(Nexus::Graphics::TextureFormat format);
    GLenum GetDepthTextureFormat(Nexus::Graphics::DepthFormat format);
}