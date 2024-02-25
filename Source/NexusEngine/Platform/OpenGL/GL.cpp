#if defined(NX_PLATFORM_OPENGL)

#include "GL.hpp"

#include "Nexus/Logging/Log.hpp"

#include <cassert>

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

    GLenum GetSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
    {
        switch (addressMode)
        {
        case Nexus::Graphics::SamplerAddressMode::Wrap:
            return GL_REPEAT;
        case Nexus::Graphics::SamplerAddressMode::MirrorOnce:
#if !defined(NX_PLATFORM_WEBGL2)
            return GL_MIRROR_CLAMP_TO_EDGE;
#else
            return GL_MIRROR_CLAMP_TO_EDGE_EXT;
#endif
        case Nexus::Graphics::SamplerAddressMode::Mirror:
            return GL_MIRRORED_REPEAT;
        case Nexus::Graphics::SamplerAddressMode::Border:
#if !defined(NX_PLATFORM_WEBGL2)
            return GL_CLAMP_TO_BORDER;
#else
            return GL_CLAMP_TO_BORDER_EXT;
#endif
        case Nexus::Graphics::SamplerAddressMode::Clamp:
            return GL_CLAMP_TO_EDGE;
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

    GLenum GetPixelType(Nexus::Graphics::PixelFormat format)
    {
        switch (format)
        {
        case Nexus::Graphics::PixelFormat::R8_UNorm:
        case Nexus::Graphics::PixelFormat::R8_UInt:
        case Nexus::Graphics::PixelFormat::R8_G8_UNorm:
        case Nexus::Graphics::PixelFormat::R8_G8_UInt:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt:
        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm:
        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB:
            return GL_UNSIGNED_BYTE;

        case Nexus::Graphics::PixelFormat::R8_SNorm:
        case Nexus::Graphics::PixelFormat::R8_SInt:
        case Nexus::Graphics::PixelFormat::R8_G8_SNorm:
        case Nexus::Graphics::PixelFormat::R8_G8_SInt:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt:
        case Nexus::Graphics::PixelFormat::BC4_SNorm:
        case Nexus::Graphics::PixelFormat::BC5_SNorm:
            return GL_BYTE;

        case Nexus::Graphics::PixelFormat::R16_UNorm:
        case Nexus::Graphics::PixelFormat::R16_UInt:
        case Nexus::Graphics::PixelFormat::R16_G16_UNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_UInt:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt:
            return GL_UNSIGNED_SHORT;

        case Nexus::Graphics::PixelFormat::R16_SNorm:
        case Nexus::Graphics::PixelFormat::R16_SInt:
        case Nexus::Graphics::PixelFormat::R16_G16_SNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_SInt:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt:
            return GL_SHORT;

        case Nexus::Graphics::PixelFormat::R32_UInt:
        case Nexus::Graphics::PixelFormat::R32_G32_UInt:
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt:
            return GL_UNSIGNED_INT;

        case Nexus::Graphics::PixelFormat::R32_SInt:
        case Nexus::Graphics::PixelFormat::R32_G32_SInt:
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt:
            return GL_INT;

        case Nexus::Graphics::PixelFormat::R16_Float:
        case Nexus::Graphics::PixelFormat::R16_G16_Float:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float:
            return GL_HALF_FLOAT;

        case Nexus::Graphics::PixelFormat::R32_Float:
        case Nexus::Graphics::PixelFormat::R32_G32_Float:
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float:
            return GL_FLOAT;

        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm:
        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm:
        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::BC2_UNorm:
        case Nexus::Graphics::PixelFormat::BC2_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::BC3_UNorm:
        case Nexus::Graphics::PixelFormat::BC3_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::BC4_UNorm:
        case Nexus::Graphics::PixelFormat::BC5_UNorm:
        case Nexus::Graphics::PixelFormat::BC7_UNorm:
        case Nexus::Graphics::PixelFormat::BC7_UNorm_SRGB:
            return GL_UNSIGNED_BYTE;

        case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt:
            return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt:
            return GL_UNSIGNED_INT_24_8;

        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm:
        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt:
#if defined(__EMSCRIPTEN__)
            throw std::runtime_error("This format is not supported by WebGL");
#else
            return GL_UNSIGNED_INT_10_10_10_2_EXT;
#endif
        case Nexus::Graphics::PixelFormat::R11_G11_B10_Float:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;

        default:
            throw std::runtime_error("Failed to find a valid format");
        }
    }

    GLenum GetPixelDataFormat(Nexus::Graphics::PixelFormat format)
    {
        switch (format)
        {
        case Nexus::Graphics::PixelFormat::R8_UNorm:
        case Nexus::Graphics::PixelFormat::R16_UNorm:
        case Nexus::Graphics::PixelFormat::R16_Float:
        case Nexus::Graphics::PixelFormat::R32_Float:
        case Nexus::Graphics::PixelFormat::BC4_UNorm:
            return GL_RED;

        case Nexus::Graphics::PixelFormat::R8_SNorm:
        case Nexus::Graphics::PixelFormat::R8_UInt:
        case Nexus::Graphics::PixelFormat::R8_SInt:
        case Nexus::Graphics::PixelFormat::R16_SNorm:
        case Nexus::Graphics::PixelFormat::R16_UInt:
        case Nexus::Graphics::PixelFormat::R16_SInt:
        case Nexus::Graphics::PixelFormat::R32_UInt:
        case Nexus::Graphics::PixelFormat::R32_SInt:
        case Nexus::Graphics::PixelFormat::BC4_SNorm:
            return GL_RED_INTEGER;

        case Nexus::Graphics::PixelFormat::R8_G8_UNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_UNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_Float:
        case Nexus::Graphics::PixelFormat::R32_G32_Float:
        case Nexus::Graphics::PixelFormat::BC5_UNorm:
            return GL_RG;

        case Nexus::Graphics::PixelFormat::R8_G8_SNorm:
        case Nexus::Graphics::PixelFormat::R8_G8_UInt:
        case Nexus::Graphics::PixelFormat::R8_G8_SInt:
        case Nexus::Graphics::PixelFormat::R16_G16_SNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_UInt:
        case Nexus::Graphics::PixelFormat::R16_G16_SInt:
        case Nexus::Graphics::PixelFormat::R32_G32_UInt:
        case Nexus::Graphics::PixelFormat::R32_G32_SInt:
        case Nexus::Graphics::PixelFormat::BC5_SNorm:
            return GL_RG_INTEGER;

        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float:
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float:
            return GL_RGBA;

        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm:
        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB:
            return GL_BGRA_EXT;

        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt:
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt:
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt:
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt:
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt:
            return GL_RGBA_INTEGER;

        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm:
        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRGB:
            return GL_RGB;

        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm:
        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::BC2_UNorm:
        case Nexus::Graphics::PixelFormat::BC3_UNorm:
        case Nexus::Graphics::PixelFormat::BC3_UNorm_SRGB:
        case Nexus::Graphics::PixelFormat::BC7_UNorm_SRGB:
            return GL_RGBA;

        case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt:
        case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt:
            return GL_DEPTH_STENCIL;

        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm:
            return GL_RGBA;
        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt:
            return GL_RGBA_INTEGER;
        case Nexus::Graphics::PixelFormat::R11_G11_B10_Float:
            return GL_RGB;

        default:
            throw std::runtime_error("Failed to find a valid format");
        }
    }

    GLenum GetSizedInternalFormat(Nexus::Graphics::PixelFormat format, bool depthFormat)
    {
        switch (format)
        {
        case Nexus::Graphics::PixelFormat::R8_UNorm:
            return GL_R8;
        case Nexus::Graphics::PixelFormat::R8_SNorm:
            return GL_R8I;
        case Nexus::Graphics::PixelFormat::R8_UInt:
            return GL_R8UI;
        case Nexus::Graphics::PixelFormat::R8_SInt:
            return GL_R8I;

        case Nexus::Graphics::PixelFormat::R16_UNorm:
            return depthFormat ? GL_DEPTH_COMPONENT16 : GL_R16_EXT;
        case Nexus::Graphics::PixelFormat::R16_SNorm:
            return GL_R16I;
        case Nexus::Graphics::PixelFormat::R16_UInt:
            return GL_R16UI;
        case Nexus::Graphics::PixelFormat::R16_SInt:
            return GL_R16I;
        case Nexus::Graphics::PixelFormat::R16_Float:
            return GL_R16F;

        case Nexus::Graphics::PixelFormat::R32_UInt:
            return GL_R32UI;
        case Nexus::Graphics::PixelFormat::R32_SInt:
            return GL_R32I;
        case Nexus::Graphics::PixelFormat::R32_Float:
            return depthFormat ? GL_DEPTH_COMPONENT32F : GL_R32F;

        case Nexus::Graphics::PixelFormat::R8_G8_UNorm:
            return GL_RG8;
        case Nexus::Graphics::PixelFormat::R8_G8_SNorm:
            return GL_RG8I;
        case Nexus::Graphics::PixelFormat::R8_G8_UInt:
            return GL_RG8UI;
        case Nexus::Graphics::PixelFormat::R8_G8_SInt:
            return GL_RG8I;

        case Nexus::Graphics::PixelFormat::R16_G16_UNorm:
            return GL_RG16_EXT;
        case Nexus::Graphics::PixelFormat::R16_G16_SNorm:
            return GL_RG16I;
        case Nexus::Graphics::PixelFormat::R16_G16_UInt:
            return GL_RG16UI;
        case Nexus::Graphics::PixelFormat::R16_G16_SInt:
            return GL_RG16I;
        case Nexus::Graphics::PixelFormat::R16_G16_Float:
            return GL_RG16F;

        case Nexus::Graphics::PixelFormat::R32_G32_UInt:
            return GL_RG32UI;
        case Nexus::Graphics::PixelFormat::R32_G32_SInt:
            return GL_RG32I;
        case Nexus::Graphics::PixelFormat::R32_G32_Float:
            return GL_RG32F;

        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm:
            return GL_RGBA8;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB:
            return GL_SRGB8_ALPHA8;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm:
            return GL_RGBA8I;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt:
            return GL_RGBA8UI;
        case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt:
            return GL_RGBA8I;
        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm:
            return GL_RGBA8;
        case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB:
            return GL_SRGB8_ALPHA8;

        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm:
            return GL_RGBA16_EXT;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm:
            return GL_RGBA16I;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt:
            return GL_RGBA16UI;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt:
            return GL_RGBA16I;
        case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float:
            return GL_RGBA16F;

        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt:
            return GL_RGBA32UI;
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt:
            return GL_RGBA32I;
        case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float:
            return GL_RGBA32F;

        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRGB:
            return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm:
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRGB:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        case Nexus::Graphics::PixelFormat::BC2_UNorm:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case Nexus::Graphics::PixelFormat::BC2_UNorm_SRGB:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case Nexus::Graphics::PixelFormat::BC3_UNorm:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case Nexus::Graphics::PixelFormat::BC3_UNorm_SRGB:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        case Nexus::Graphics::PixelFormat::BC4_UNorm:
            return GL_COMPRESSED_RED_RGTC1_EXT;
        case Nexus::Graphics::PixelFormat::BC4_SNorm:
            return GL_COMPRESSED_SIGNED_RED_RGTC1_EXT;
        case Nexus::Graphics::PixelFormat::BC5_UNorm:
            return GL_COMPRESSED_RED_GREEN_RGTC2_EXT;
        case Nexus::Graphics::PixelFormat::BC5_SNorm:
            return GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT;
        case Nexus::Graphics::PixelFormat::BC7_UNorm:
            return GL_COMPRESSED_RGBA_BPTC_UNORM_EXT;
        case Nexus::Graphics::PixelFormat::BC7_UNorm_SRGB:
            return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT;

        case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt:
            assert(depthFormat);
            return GL_DEPTH32F_STENCIL8;
        case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt:
            assert(depthFormat);
            return GL_DEPTH24_STENCIL8;

        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm:
            return GL_RGB10_A2;
        case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt:
            return GL_RGB10_A2UI;
        case Nexus::Graphics::PixelFormat::R11_G11_B10_Float:
            return GL_R11F_G11F_B10F;

        default:
            throw std::runtime_error("Failed to find a valid format");
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