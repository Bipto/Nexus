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

    GLenum GetStencilOperation(Nexus::Graphics::StencilOperation operation)
    {
        switch (operation)
        {
        case Nexus::Graphics::StencilOperation::Keep:
            return GL_KEEP;
        case Nexus::Graphics::StencilOperation::Zero:
            return GL_ZERO;
        case Nexus::Graphics::StencilOperation::Replace:
            return GL_REPLACE;
        case Nexus::Graphics::StencilOperation::Increment:
            return GL_INCR;
        case Nexus::Graphics::StencilOperation::Decrement:
            return GL_DECR;
        case Nexus::Graphics::StencilOperation::Invert:
            return GL_INVERT;
        }

        throw std::runtime_error("Failed to find a valid stencil operation");
    }

    GLenum GetComparisonFunction(Nexus::Graphics::ComparisonFunction function)
    {
        switch (function)
        {
        case Nexus::Graphics::ComparisonFunction::Always:
            return GL_ALWAYS;
        case Nexus::Graphics::ComparisonFunction::Equal:
            return GL_EQUAL;
        case Nexus::Graphics::ComparisonFunction::Greater:
            return GL_GREATER;
        case Nexus::Graphics::ComparisonFunction::GreaterEqual:
            return GL_GEQUAL;
        case Nexus::Graphics::ComparisonFunction::Less:
            return GL_LESS;
        case Nexus::Graphics::ComparisonFunction::LessEqual:
            return GL_LEQUAL;
        case Nexus::Graphics::ComparisonFunction::Never:
            return GL_NEVER;
        case Nexus::Graphics::ComparisonFunction::NotEqual:
            return GL_NOTEQUAL;
        }

        throw std::runtime_error("Failed to find a valid comparison function");
    }

    GLenum GetBlendFunction(Nexus::Graphics::BlendFunction function)
    {
        switch (function)
        {
        case Nexus::Graphics::BlendFunction::Zero:
            return GL_ZERO;
        case Nexus::Graphics::BlendFunction::One:
            return GL_ONE;
        case Nexus::Graphics::BlendFunction::SourceColor:
            return GL_SRC_COLOR;
        case Nexus::Graphics::BlendFunction::OneMinusSourceColor:
            return GL_ONE_MINUS_SRC_COLOR;
        case Nexus::Graphics::BlendFunction::DestinationColor:
            return GL_DST_COLOR;
        case Nexus::Graphics::BlendFunction::OneMinusDestinationColor:
            return GL_ONE_MINUS_DST_COLOR;
        case Nexus::Graphics::BlendFunction::SourceAlpha:
            return GL_SRC_ALPHA;
        case Nexus::Graphics::BlendFunction::OneMinusSourceAlpha:
            return GL_ONE_MINUS_SRC_ALPHA;
        case Nexus::Graphics::BlendFunction::DestinationAlpha:
            return GL_DST_ALPHA;
        case Nexus::Graphics::BlendFunction::OneMinusDestinationAlpha:
            return GL_ONE_MINUS_DST_ALPHA;
        }

        throw std::runtime_error("Failed to find a valid blend function");
    }

    GLenum GetBlendEquation(Nexus::Graphics::BlendEquation equation)
    {
        switch (equation)
        {
        case Nexus::Graphics::BlendEquation::Add:
            return GL_FUNC_ADD;
        case Nexus::Graphics::BlendEquation::Subtract:
            return GL_FUNC_SUBTRACT;
        case Nexus::Graphics::BlendEquation::ReverseSubtract:
            return GL_FUNC_REVERSE_SUBTRACT;
        case Nexus::Graphics::BlendEquation::Min:
            return GL_MIN;
        case Nexus::Graphics::BlendEquation::Max:
            return GL_MAX;
        }

        throw std::runtime_error("Failed to find a valid blend equation");
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

    GLenum GetSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
    {
        switch (addressMode)
        {
        case Nexus::Graphics::SamplerAddressMode::Wrap:
            return GL_REPEAT;
        case Nexus::Graphics::SamplerAddressMode::Mirror:
            return GL_MIRRORED_REPEAT;
        case Nexus::Graphics::SamplerAddressMode::MirrorOnce:
            return GL_MIRROR_CLAMP_TO_EDGE;
        case Nexus::Graphics::SamplerAddressMode::Clamp:
            return GL_CLAMP_TO_EDGE;
        case Nexus::Graphics::SamplerAddressMode::Border:
            return GL_CLAMP_TO_BORDER;
        default:
            throw std::runtime_error("Failed to find a valid address mode");
        }
        return GLenum();
    }

    void GetSamplerFilter(Nexus::Graphics::SamplerFilter filter, GLenum &min, GLenum &max)
    {
        switch (filter)
        {
        case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint:
            min = GL_NEAREST_MIPMAP_NEAREST;
            max = GL_NEAREST_MIPMAP_NEAREST;
            break;

        case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipLinear:
            min = GL_NEAREST_MIPMAP_LINEAR;
            max = GL_NEAREST_MIPMAP_LINEAR;
            break;

        case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipPoint:
            min = GL_NEAREST_MIPMAP_NEAREST;
            max = GL_LINEAR_MIPMAP_NEAREST;
            break;

        case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipLinear:
            min = GL_NEAREST_MIPMAP_LINEAR;
            max = GL_LINEAR_MIPMAP_LINEAR;
            break;

        case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipPoint:
            min = GL_LINEAR_MIPMAP_NEAREST;
            max = GL_NEAREST_MIPMAP_NEAREST;
            break;

        case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipLinear:
            min = GL_LINEAR_MIPMAP_LINEAR;
            max = GL_NEAREST_MIPMAP_LINEAR;
            break;

        case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipPoint:
            min = GL_LINEAR_MIPMAP_NEAREST;
            max = GL_LINEAR_MIPMAP_NEAREST;
            break;

        case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear:
        case Nexus::Graphics::SamplerFilter::Anisotropic:
            min = GL_LINEAR_MIPMAP_LINEAR;
            max = GL_LINEAR_MIPMAP_LINEAR;
            break;

        default:
            throw std::runtime_error("Failed to find a valid sample filter");
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