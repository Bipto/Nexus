#pragma once

#if defined(NX_PLATFORM_OPENGL)

#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl32.h>
#else
#include "glad/glad.h"
#endif

#include "Nexus/Graphics/TextureFormat.hpp"
#include "Nexus/Graphics/DepthFormat.hpp"
#include "Nexus/Graphics/Texture.hpp"

namespace Nexus::GL
{
    void ClearErrors();
    bool CheckErrors();
    std::string GetErrorMessageFromCode(const GLenum error);

    GLenum GetColorTextureFormat(Nexus::Graphics::TextureFormat format);
    GLenum GetInternalTextureFormat(Nexus::Graphics::TextureFormat format);
    GLenum GetTextureFormatBaseType(Nexus::Graphics::TextureFormat format);

    GLenum GetDepthTextureFormat(Nexus::Graphics::DepthFormat format);
    GLenum GetSamplerState(Nexus::Graphics::SamplerState state);
    GLenum GetWrapMode(Nexus::Graphics::SamplerState state);
}

#endif