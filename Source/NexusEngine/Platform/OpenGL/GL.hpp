#pragma once

#if defined(NX_PLATFORM_OPENGL)

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#if defined(__EMSCRIPTEN__) || defined(ANDROID) || defined(__ANDROID__)
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#else
#include "glad/glad.h"
#endif

#include "Nexus/Graphics/Texture.hpp"
#include "Nexus/Graphics/ShaderModule.hpp"
#include "Nexus/Vertex.hpp"

#include "GL.hpp"

namespace Nexus::GL
{
    void ClearErrors();
    bool CheckErrors();
    std::string GetErrorMessageFromCode(const GLenum error);

    GLenum GetStencilOperation(Nexus::Graphics::StencilOperation operation);
    GLenum GetComparisonFunction(Nexus::Graphics::ComparisonFunction function);
    GLenum GetBlendFactor(Nexus::Graphics::BlendFactor function);
    GLenum GetBlendFunction(Nexus::Graphics::BlendEquation equation);

    GLenum GetSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);
    void GetSamplerFilter(Nexus::Graphics::SamplerFilter filter, GLenum &min, GLenum &max);
    GLenum GetPixelType(Nexus::Graphics::PixelFormat format);
    GLenum GetPixelDataFormat(Nexus::Graphics::PixelFormat format);
    GLenum GetSizedInternalFormat(Nexus::Graphics::PixelFormat format, bool depthFormat);

    GLenum GetShaderStage(Nexus::Graphics::ShaderStage stage);

    void GetBaseType(const Graphics::VertexBufferElement &element, GLenum &baseType, uint32_t &componentCount, GLboolean &normalized);
}

#endif