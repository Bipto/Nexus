#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"

#include "Nexus/Logging/Log.hpp"

namespace Nexus::GL
{
    void ClearErrors()
    {
        while (glGetError() != GL_NO_ERROR)
            ;
    }

    bool CheckErrors()
    {
        bool errorEncountered = false;

        while (GLenum error = glGetError())
        {
            NX_ERROR(GetErrorMessageFromCode(error));
            errorEncountered = true;
        }

        return errorEncountered;
    }

    std::string GetErrorMessageFromCode(const GLenum error)
    {
        switch (error)
        {
        case GL_NO_ERROR:
            return {"No error"};
        case GL_INVALID_ENUM:
            return {"An invalid enum was entered"};
        case GL_INVALID_VALUE:
            return {"An invalid value was entered"};
        case GL_INVALID_OPERATION:
            return {"An invalid operation was attempted"};
        /* case GL_STACK_OVERFLOW:
            return {"A stack overflow has occured"};
        case GL_STACK_UNDERFLOW:
            return {"A stack underflow has occured"}; */
        case GL_OUT_OF_MEMORY:
            return {"Out of memory"};
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return {"An invalid framebuffer operation was attempted"};
        default:
            return {"An unknown error occurred"};
        }
    }

    GLenum GetColorTextureFormat(Nexus::Graphics::TextureFormat format)
    {
        switch (format)
        {
        case Graphics::TextureFormat::RGBA8:
            return GL_RGBA8;
        case Graphics::TextureFormat::R8:
            return GL_R8;
        default:
            throw std::runtime_error("Invalid texture format selected");
        }
    }

    GLenum GetInternalTextureFormat(Nexus::Graphics::TextureFormat format)
    {
        switch (format)
        {
        case Graphics::TextureFormat::RGBA8:
            return GL_RGBA;
        case Graphics::TextureFormat::R8:
            return GL_RED;
        default:
            throw std::runtime_error("Invalid texture format selected");
        }
    }

    GLenum GetTextureFormatBaseType(Nexus::Graphics::TextureFormat format)
    {
        switch (format)
        {
        case Graphics::TextureFormat::RGBA8:
        case Graphics::TextureFormat::R8:
            return GL_UNSIGNED_BYTE;
        default:
            throw std::runtime_error("Invalid texture format selected");
        }
    }

    GLenum GetDepthTextureFormat(Nexus::Graphics::DepthFormat format)
    {
        switch (format)
        {
        case Graphics::DepthFormat::DEPTH24STENCIL8:
            return GL_DEPTH24_STENCIL8;
        default:
            throw std::runtime_error("Invalid depth format selected");
        }
    }

    GLenum GetSamplerState(Nexus::Graphics::SamplerState state)
    {
        switch (state)
        {
        case Nexus::Graphics::SamplerState::LinearClamp:
        case Nexus::Graphics::SamplerState::LinearWrap:
            return GL_LINEAR;
        case Nexus::Graphics::SamplerState::PointClamp:
        case Nexus::Graphics::SamplerState::PointWrap:
            return GL_NEAREST;
        default:
            throw std::runtime_error("Invalid sampler state selected");
        }
    }

    GLenum GetWrapMode(Nexus::Graphics::SamplerState state)
    {
        switch (state)
        {
        case Nexus::Graphics::SamplerState::LinearClamp:
        case Nexus::Graphics::SamplerState::PointClamp:
            return GL_CLAMP_TO_EDGE;
        case Nexus::Graphics::SamplerState::LinearWrap:
        case Nexus::Graphics::SamplerState::PointWrap:
            return GL_REPEAT;
        default:
            throw std::runtime_error("Invalid sampler state selected");
        }
    }
}

#endif