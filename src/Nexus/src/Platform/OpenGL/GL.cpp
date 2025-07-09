#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"

	#include "Nexus-Core/Logging/Log.hpp"

	#if defined(NX_PLATFORM_WGL)
		#include "Context/WGL/OffscreenContextWGL.hpp"
		#include "Context/WGL/PhysicalDeviceWGL.hpp"
		#include "Context/WGL/ViewContextWGL.hpp"
	#elif defined(NX_PLATFORM_EGL)
		#include "Context/EGL/OffscreenContextEGL.hpp"
		#include "Context/EGL/ViewContextEGL.hpp"
	#elif defined(NX_PLATFORM_WEBGL)
		#include "Context/WebGL/OffscreenContextWebGL.hpp"
		#include "Context/WebGL/ViewContextWebGL.hpp"
	#elif defined(NX_PLATFORM_GLX)
		#include "Context/GLX/OffscreenContextGLX.hpp"
		#include "Context/GLX/ViewContextGLX.hpp"
	#endif

	#include "DeviceBufferOpenGL.hpp"
	#include "Nexus-Core/Platform.hpp"
	#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"
	#include "TextureOpenGL.hpp"

namespace Nexus::GL
{
	std::string GetErrorMessageFromCode(const GLenum error)
	{
		switch (error)
		{
			case GL_NO_ERROR: return "No error";
			case GL_INVALID_ENUM: return "An invalid enum was entered";
			case GL_INVALID_VALUE: return "An invalid value was entered";
			case GL_INVALID_OPERATION: return "An invalid operation was attempted";
			/* case GL_STACK_OVERFLOW:
				return "A stack overflow has occured";
			case GL_STACK_UNDERFLOW:
				return "A stack underflow has occured"; */
			case GL_OUT_OF_MEMORY: return "Out of memory";
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "An invalid framebuffer operation was attempted";
			default: return "An unknown error occurred";
		}
	}

	GLenum GetStencilOperation(Nexus::Graphics::StencilOperation operation)
	{
		switch (operation)
		{
			case Nexus::Graphics::StencilOperation::Keep: return GL_KEEP;
			case Nexus::Graphics::StencilOperation::Zero: return GL_ZERO;
			case Nexus::Graphics::StencilOperation::Replace: return GL_REPLACE;
			case Nexus::Graphics::StencilOperation::Increment: return GL_INCR;
			case Nexus::Graphics::StencilOperation::Decrement: return GL_DECR;
			case Nexus::Graphics::StencilOperation::Invert: return GL_INVERT;
		}

		throw std::runtime_error("Failed to find a valid stencil operation");
	}

	GLenum GetComparisonFunction(Nexus::Graphics::ComparisonFunction function)
	{
		switch (function)
		{
			case Nexus::Graphics::ComparisonFunction::AlwaysPass: return GL_ALWAYS;
			case Nexus::Graphics::ComparisonFunction::Equal: return GL_EQUAL;
			case Nexus::Graphics::ComparisonFunction::Greater: return GL_GREATER;
			case Nexus::Graphics::ComparisonFunction::GreaterEqual: return GL_GEQUAL;
			case Nexus::Graphics::ComparisonFunction::Less: return GL_LESS;
			case Nexus::Graphics::ComparisonFunction::LessEqual: return GL_LEQUAL;
			case Nexus::Graphics::ComparisonFunction::Never: return GL_NEVER;
			case Nexus::Graphics::ComparisonFunction::NotEqual: return GL_NOTEQUAL;
		}

		throw std::runtime_error("Failed to find a valid comparison function");
	}

	GLenum GetBlendFactor(Nexus::Graphics::BlendFactor function)
	{
		switch (function)
		{
			case Nexus::Graphics::BlendFactor::Zero: return GL_ZERO;
			case Nexus::Graphics::BlendFactor::One: return GL_ONE;
			case Nexus::Graphics::BlendFactor::SourceColour: return GL_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusSourceColour: return GL_ONE_MINUS_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::DestinationColour: return GL_DST_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationColour: return GL_ONE_MINUS_DST_COLOR;
			case Nexus::Graphics::BlendFactor::SourceAlpha: return GL_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::DestinationAlpha: return GL_DST_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
			case Nexus::Graphics::BlendFactor::FactorColour: return GL_CONSTANT_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusFactorColour: return GL_ONE_MINUS_CONSTANT_COLOR;
			case Nexus::Graphics::BlendFactor::FactorAlpha: return GL_CONSTANT_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusFactorAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
		}

		throw std::runtime_error("Failed to find a valid blend function");
	}

	GLenum GetBlendFunction(Nexus::Graphics::BlendEquation equation)
	{
		switch (equation)
		{
			case Nexus::Graphics::BlendEquation::Add: return GL_FUNC_ADD;
			case Nexus::Graphics::BlendEquation::Subtract: return GL_FUNC_SUBTRACT;
			case Nexus::Graphics::BlendEquation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
			case Nexus::Graphics::BlendEquation::Min: return GL_MIN;
			case Nexus::Graphics::BlendEquation::Max: return GL_MAX;
		}

		throw std::runtime_error("Failed to find a valid blend equation");
	}

	GLenum GetSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode)
	{
		switch (addressMode)
		{
			case Nexus::Graphics::SamplerAddressMode::Wrap: return GL_REPEAT;
			case Nexus::Graphics::SamplerAddressMode::MirrorOnce: return GL_MIRROR_CLAMP_TO_EDGE_EXT;
			case Nexus::Graphics::SamplerAddressMode::Mirror: return GL_MIRRORED_REPEAT;
			case Nexus::Graphics::SamplerAddressMode::Border: return GL_CLAMP_TO_BORDER;
			case Nexus::Graphics::SamplerAddressMode::Clamp: return GL_CLAMP_TO_EDGE;
			default: throw std::runtime_error("Failed to find a valid address mode");
		}
		return GLenum();
	}

	void GetSamplerFilter(Nexus::Graphics::SamplerFilter filter, GLenum &min, GLenum &max, bool hasMipmaps)
	{
		switch (filter)
		{
			case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipPoint:
				hasMipmaps ? min = GL_NEAREST_MIPMAP_NEAREST : min = GL_NEAREST;
				max = GL_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinPoint_MagPoint_MipLinear:
				hasMipmaps ? min = GL_NEAREST_MIPMAP_LINEAR : min = GL_NEAREST;
				max = GL_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipPoint:
				hasMipmaps ? min = GL_NEAREST_MIPMAP_NEAREST : min = GL_NEAREST;
				max = GL_LINEAR;
				break;

			case Nexus::Graphics::SamplerFilter::MinPoint_MagLinear_MipLinear:
				hasMipmaps ? min = GL_NEAREST_MIPMAP_NEAREST : min = GL_NEAREST;
				max = GL_LINEAR;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipPoint:
				hasMipmaps ? min = GL_LINEAR_MIPMAP_NEAREST : min = GL_LINEAR;
				max = GL_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagPoint_MipLinear:
				hasMipmaps ? min = GL_LINEAR_MIPMAP_LINEAR : min = GL_LINEAR;
				max = GL_NEAREST;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipPoint:
				hasMipmaps ? min = GL_LINEAR_MIPMAP_NEAREST : min = GL_LINEAR;
				max = GL_LINEAR;
				break;

			case Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear:
			case Nexus::Graphics::SamplerFilter::Anisotropic:
				hasMipmaps ? min = GL_LINEAR_MIPMAP_LINEAR : min = GL_LINEAR;
				max = GL_LINEAR;
				break;

			default: throw std::runtime_error("Failed to find a valid sample filter");
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
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB: return GL_UNSIGNED_BYTE;

			case Nexus::Graphics::PixelFormat::R16_UNorm:
			case Nexus::Graphics::PixelFormat::R16_UInt:
			case Nexus::Graphics::PixelFormat::R16_G16_UNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_UInt:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt: return GL_UNSIGNED_SHORT;

			case Nexus::Graphics::PixelFormat::R16_SNorm:
			case Nexus::Graphics::PixelFormat::R16_SInt:
			case Nexus::Graphics::PixelFormat::R16_G16_SNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_SInt:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt: return GL_SHORT;

			case Nexus::Graphics::PixelFormat::R32_UInt:
			case Nexus::Graphics::PixelFormat::R32_G32_UInt:
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt: return GL_UNSIGNED_INT;

			case Nexus::Graphics::PixelFormat::R32_SInt:
			case Nexus::Graphics::PixelFormat::R32_G32_SInt:
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt: return GL_INT;

			case Nexus::Graphics::PixelFormat::R16_Float:
			case Nexus::Graphics::PixelFormat::R16_G16_Float:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float: return GL_HALF_FLOAT;

			case Nexus::Graphics::PixelFormat::R32_Float:
			case Nexus::Graphics::PixelFormat::R32_G32_Float:
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float: return GL_FLOAT;

			case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt: return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
			case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt: return GL_UNSIGNED_INT_24_8;

			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm:
			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt:
	#if defined(__EMSCRIPTEN__)
				throw std::runtime_error("This format is not supported by WebGL");
	#elif defined(__ANDROID__) || defined(ANDROID)
				throw std::runtime_error("This format is not supported by OpenGLES");
	#else
				return GL_UNSIGNED_INT_10_10_10_2_EXT;
	#endif
			case Nexus::Graphics::PixelFormat::R11_G11_B10_Float: return GL_UNSIGNED_INT_10F_11F_11F_REV;

			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A1_UNorm:
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A8_UNorm:
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_UNorm:
			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm:
			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm:
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC2_UNorm:
			case Nexus::Graphics::PixelFormat::BC2_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC3_UNorm:
			case Nexus::Graphics::PixelFormat::BC3_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC4_UNorm:
			case Nexus::Graphics::PixelFormat::BC4_SNorm:
			case Nexus::Graphics::PixelFormat::BC5_UNorm:
			case Nexus::Graphics::PixelFormat::BC5_SNorm:
			case Nexus::Graphics::PixelFormat::BC7_UNorm:
			case Nexus::Graphics::PixelFormat::BC7_UNorm_SRgb: return GL_UNSIGNED_BYTE;

			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	GLenum GetPixelDataFormat(Nexus::Graphics::PixelFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::PixelFormat::R8_UNorm:
			case Nexus::Graphics::PixelFormat::R16_UNorm:
			case Nexus::Graphics::PixelFormat::R16_Float:
			case Nexus::Graphics::PixelFormat::R32_Float: return GL_RED;

			case Nexus::Graphics::PixelFormat::R8_SNorm:
			case Nexus::Graphics::PixelFormat::R8_UInt:
			case Nexus::Graphics::PixelFormat::R8_SInt:
			case Nexus::Graphics::PixelFormat::R16_SNorm:
			case Nexus::Graphics::PixelFormat::R16_UInt:
			case Nexus::Graphics::PixelFormat::R16_SInt:
			case Nexus::Graphics::PixelFormat::R32_UInt:
			case Nexus::Graphics::PixelFormat::R32_SInt: return GL_RED_INTEGER;

			case Nexus::Graphics::PixelFormat::R8_G8_UNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_UNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_Float:
			case Nexus::Graphics::PixelFormat::R32_G32_Float: return GL_RG;

			case Nexus::Graphics::PixelFormat::R8_G8_SNorm:
			case Nexus::Graphics::PixelFormat::R8_G8_UInt:
			case Nexus::Graphics::PixelFormat::R8_G8_SInt:
			case Nexus::Graphics::PixelFormat::R16_G16_SNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_UInt:
			case Nexus::Graphics::PixelFormat::R16_G16_SInt:
			case Nexus::Graphics::PixelFormat::R32_G32_UInt:
			case Nexus::Graphics::PixelFormat::R32_G32_SInt: return GL_RG_INTEGER;

			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm:
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float:
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float: return GL_RGBA;

			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm:
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB: return GL_BGRA_EXT;

			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm:
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt:
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt:
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt:
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt:
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt: return GL_RGBA_INTEGER;

			case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt:
			case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt: return GL_DEPTH_STENCIL;

			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm: return GL_RGBA;
			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt: return GL_RGBA_INTEGER;
			case Nexus::Graphics::PixelFormat::R11_G11_B10_Float: return GL_RGB;

			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm:
			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_UNorm: return GL_RGB;

			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A1_UNorm:
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A8_UNorm:
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm:
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC2_UNorm:
			case Nexus::Graphics::PixelFormat::BC2_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC3_UNorm:
			case Nexus::Graphics::PixelFormat::BC3_UNorm_SRgb:
			case Nexus::Graphics::PixelFormat::BC4_UNorm:
			case Nexus::Graphics::PixelFormat::BC4_SNorm:
			case Nexus::Graphics::PixelFormat::BC5_UNorm:
			case Nexus::Graphics::PixelFormat::BC5_SNorm:
			case Nexus::Graphics::PixelFormat::BC7_UNorm:
			case Nexus::Graphics::PixelFormat::BC7_UNorm_SRgb: return GL_RGBA;

			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	GLenum GetSizedInternalFormat(Nexus::Graphics::PixelFormat format, bool depthFormat)
	{
		switch (format)
		{
			case Nexus::Graphics::PixelFormat::R8_UNorm: return GL_R8;
			case Nexus::Graphics::PixelFormat::R8_SNorm: return GL_R8I;
			case Nexus::Graphics::PixelFormat::R8_UInt: return GL_R8UI;
			case Nexus::Graphics::PixelFormat::R8_SInt: return GL_R8I;

			case Nexus::Graphics::PixelFormat::R16_UNorm: return depthFormat ? GL_DEPTH_COMPONENT16 : GL_R16;
			case Nexus::Graphics::PixelFormat::R16_SNorm: return GL_R16I;
			case Nexus::Graphics::PixelFormat::R16_UInt: return GL_R16UI;
			case Nexus::Graphics::PixelFormat::R16_SInt: return GL_R16I;
			case Nexus::Graphics::PixelFormat::R16_Float: return GL_R16F;

			case Nexus::Graphics::PixelFormat::R32_UInt: return GL_R32UI;
			case Nexus::Graphics::PixelFormat::R32_SInt: return GL_R32I;
			case Nexus::Graphics::PixelFormat::R32_Float: return depthFormat ? GL_DEPTH_COMPONENT32F : GL_R32F;

			case Nexus::Graphics::PixelFormat::R8_G8_UNorm: return GL_RG8;
			case Nexus::Graphics::PixelFormat::R8_G8_SNorm: return GL_RG8I;
			case Nexus::Graphics::PixelFormat::R8_G8_UInt: return GL_RG8UI;
			case Nexus::Graphics::PixelFormat::R8_G8_SInt: return GL_RG8I;

			case Nexus::Graphics::PixelFormat::R16_G16_UNorm: return GL_RG16;
			case Nexus::Graphics::PixelFormat::R16_G16_SNorm: return GL_RG16I;
			case Nexus::Graphics::PixelFormat::R16_G16_UInt: return GL_RG16UI;
			case Nexus::Graphics::PixelFormat::R16_G16_SInt: return GL_RG16I;
			case Nexus::Graphics::PixelFormat::R16_G16_Float: return GL_RG16F;

			case Nexus::Graphics::PixelFormat::R32_G32_UInt: return GL_RG32UI;
			case Nexus::Graphics::PixelFormat::R32_G32_SInt: return GL_RG32I;
			case Nexus::Graphics::PixelFormat::R32_G32_Float: return GL_RG32F;

			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm: return GL_RGBA8;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB: return GL_SRGB8_ALPHA8;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SNorm: return GL_RGBA8I;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UInt: return GL_RGBA8UI;
			case Nexus::Graphics::PixelFormat::R8_G8_B8_A8_SInt: return GL_RGBA8I;
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm: return GL_RGBA8;
			case Nexus::Graphics::PixelFormat::B8_G8_R8_A8_UNorm_SRGB: return GL_SRGB8_ALPHA8;

			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UNorm: return GL_RGBA16_EXT;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SNorm: return GL_RGBA16I;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_UInt: return GL_RGBA16UI;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_SInt: return GL_RGBA16I;
			case Nexus::Graphics::PixelFormat::R16_G16_B16_A16_Float: return GL_RGBA16F;

			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_UInt: return GL_RGBA32UI;
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_SInt: return GL_RGBA32I;
			case Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float: return GL_RGBA32F;

			case Nexus::Graphics::PixelFormat::D32_Float_S8_UInt: assert(depthFormat); return GL_DEPTH32F_STENCIL8;
			case Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt: assert(depthFormat); return GL_DEPTH24_STENCIL8;

			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UNorm: return GL_RGB10_A2;
			case Nexus::Graphics::PixelFormat::R10_G10_B10_A2_UInt: return GL_RGB10_A2UI;
			case Nexus::Graphics::PixelFormat::R11_G11_B10_Float: return GL_R11F_G11F_B10F;

			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A1_UNorm: return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_A8_UNorm: return GL_COMPRESSED_RGBA;
			case Nexus::Graphics::PixelFormat::ETC2_R8_G8_B8_UNorm: return GL_COMPRESSED_RGB8_ETC2;
			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			case Nexus::Graphics::PixelFormat::BC1_Rgb_UNorm_SRgb: return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			case Nexus::Graphics::PixelFormat::BC1_Rgba_UNorm_SRgb: return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			case Nexus::Graphics::PixelFormat::BC2_UNorm: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			case Nexus::Graphics::PixelFormat::BC2_UNorm_SRgb: return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			case Nexus::Graphics::PixelFormat::BC3_UNorm: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			case Nexus::Graphics::PixelFormat::BC3_UNorm_SRgb: return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			case Nexus::Graphics::PixelFormat::BC4_UNorm: return GL_COMPRESSED_RED_RGTC1;
			case Nexus::Graphics::PixelFormat::BC4_SNorm: return GL_COMPRESSED_SIGNED_RED_RGTC1;
			case Nexus::Graphics::PixelFormat::BC5_UNorm: return GL_COMPRESSED_RG_RGTC2;
			case Nexus::Graphics::PixelFormat::BC5_SNorm: return GL_COMPRESSED_SIGNED_RG_RGTC2;
			case Nexus::Graphics::PixelFormat::BC7_UNorm: return GL_COMPRESSED_RGBA_BPTC_UNORM;
			case Nexus::Graphics::PixelFormat::BC7_UNorm_SRgb: return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;

			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	GLenum GetGLIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
	{
		switch (format)
		{
			case Nexus::Graphics::IndexBufferFormat::UInt8: return GL_UNSIGNED_BYTE;
			case Nexus::Graphics::IndexBufferFormat::UInt16: return GL_UNSIGNED_SHORT;
			case Nexus::Graphics::IndexBufferFormat::UInt32: return GL_UNSIGNED_INT;
			default: throw std::runtime_error("Failed to find a valid index buffer format");
		}
	}

	GLenum GetTopology(Nexus::Graphics::Topology topology)
	{
		switch (topology)
		{
			case Nexus::Graphics::Topology::LineList: return GL_LINE_LOOP;
			case Nexus::Graphics::Topology::LineStrip: return GL_LINE_STRIP;
			case Nexus::Graphics::Topology::PointList: return GL_POINTS;
			case Nexus::Graphics::Topology::TriangleList: return GL_TRIANGLES;
			case Nexus::Graphics::Topology::TriangleStrip: return GL_TRIANGLE_STRIP;
			default: throw std::runtime_error("Invalid topology selected");
		}
	}

	GLenum GetShaderStage(Nexus::Graphics::ShaderStage stage)
	{
		switch (stage)
		{
			case Nexus::Graphics::ShaderStage::Compute: return GL_COMPUTE_SHADER;
			case Nexus::Graphics::ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
			case Nexus::Graphics::ShaderStage::TesselationControl: return GL_TESS_CONTROL_SHADER;
			case Nexus::Graphics::ShaderStage::TesselationEvaluation: return GL_TESS_EVALUATION_SHADER;
			case Nexus::Graphics::ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
			case Nexus::Graphics::ShaderStage::Vertex: return GL_VERTEX_SHADER;

			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}

	GLenum GetBufferUsage(const Graphics::DeviceBufferDescription &desc)
	{
		switch (desc.Access)
		{
			case Graphics::BufferMemoryAccess::Upload: return GL_DYNAMIC_COPY;
			case Graphics::BufferMemoryAccess::Default: return GL_DYNAMIC_DRAW;
			case Graphics::BufferMemoryAccess::Readback: return GL_DYNAMIC_COPY;
			default: throw std::runtime_error("Failed to find a valid access");
		}

		return GL_STATIC_DRAW;
	}

	GLenum GetAccessMask(Graphics::ShaderAccess access)
	{
		switch (access)
		{
			case Graphics::ShaderAccess::Read: return GL_READ_ONLY;
			case Graphics::ShaderAccess::ReadWrite: return GL_READ_WRITE;
			default: throw std::runtime_error("Failed to find a valid access mask");
		}
	}

	GLenum GetTextureType(const Graphics::TextureSpecification &spec)
	{
		switch (spec.Type)
		{
			case Graphics::TextureType::Texture1D:
			{
	#if !defined(__EMSCRIPTEN__)
				if (spec.ArrayLayers > 1)
				{
					return GL_TEXTURE_1D_ARRAY;
				}
				else
				{
					return GL_TEXTURE_1D;
				}
	#else
				throw std::runtime_error("1D textures are not supported by WebGL");
	#endif
			}
			case Graphics::TextureType::Texture2D:
			{
				if (spec.Usage & Graphics::TextureUsage_Cubemap)
				{
					if (spec.ArrayLayers > 6)
					{
						return GL_TEXTURE_CUBE_MAP_ARRAY;
					}
					else
					{
						return GL_TEXTURE_CUBE_MAP;
					}
				}
				else
				{
					if (spec.ArrayLayers > 1)
					{
						return GL_TEXTURE_2D_ARRAY;
					}
					else
					{
						return GL_TEXTURE_2D;
					}
				}
			}
			case Graphics::TextureType::Texture3D:
			{
				return GL_TEXTURE_3D;
			}
			default: throw std::runtime_error("Failed to find a valid texture type");
		}
	}

	GLInternalTextureFormat GetGLInternalTextureFormat(const Graphics::TextureSpecification &spec)
	{
		switch (spec.Type)
		{
			case Graphics::TextureType::Texture1D:
			{
				if (spec.ArrayLayers > 1)
				{
					return GLInternalTextureFormat::Texture1DArray;
				}
				else
				{
					return GLInternalTextureFormat::Texture1D;
				}
			}

			case Graphics::TextureType::Texture2D:
			{
				if (spec.Usage & Graphics::TextureUsage_Cubemap)
				{
					if (spec.ArrayLayers > 6)
					{
						return GLInternalTextureFormat::CubemapArray;
					}
					else
					{
						return GLInternalTextureFormat::Cubemap;
					}
				}
				else
				{
					if (spec.Samples > 1)
					{
						if (spec.ArrayLayers > 1)
						{
							return GLInternalTextureFormat::Texture2DArrayMultisample;
						}
						else
						{
							return GLInternalTextureFormat::Texture2DMultisample;
						}
					}
					else
					{
						if (spec.ArrayLayers > 1)
						{
							return GLInternalTextureFormat::Texture2DArray;
						}
						else
						{
							return GLInternalTextureFormat::Texture2D;
						}
					}
				}
			}
			case Graphics::TextureType::Texture3D:
			{
				return GLInternalTextureFormat::Texture3D;
			}
			default: throw std::runtime_error("Failed to find a valid GLInternalTextureFormat");
		}
	}

	void ValidateFramebuffer(GLuint framebuffer, const GladGLContext &context)
	{
		context.BindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		GLenum status = context.CheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status == GL_FRAMEBUFFER_COMPLETE)
		{
			return;
		}

		switch (status)
		{
			case GL_FRAMEBUFFER_UNDEFINED:
				throw std::runtime_error(
					"The specified framebuffer is the default read or write framebuffer but the default framebuffer does not exist");
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: throw std::runtime_error("An attachment is incomplete");
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: throw std::runtime_error("This framebuffer does not have any attachments");

	#if !defined(__EMSCRIPTEN__)
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: throw std::runtime_error("The framebuffer does not have a draw buffer");
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: throw std::runtime_error("The framebuffer does not have a read buffer");
	#endif

			case GL_FRAMEBUFFER_UNSUPPORTED: throw std::runtime_error("The framebuffer pixel format(s) are unsupported");
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: throw std::runtime_error("The attachments have mismatching multisample levels");
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: throw std::runtime_error("Framebuffer attachment is layered");
		}
	}

	void AttachTextureNonDSA(GLuint						  framebuffer,
							 Ref<Graphics::TextureOpenGL> texture,
							 uint32_t					  mipLevel,
							 uint32_t					  arrayLayer,
							 Graphics::ImageAspect		  aspect,
							 uint32_t					  colourIndex,
							 const GladGLContext		 &context)
	{
		glCall(context.BindFramebuffer(GL_FRAMEBUFFER, framebuffer));
		GLenum attachmentType = GL::GetAttachmentType(aspect, colourIndex);

		uint32_t				textureHandle  = texture->GetHandle();
		GLenum					textureTarget  = texture->GetTextureType();
		GLInternalTextureFormat internalFormat = texture->GetInternalGLTextureFormat();

		switch (internalFormat)
		{
			case GLInternalTextureFormat::Texture1D:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.FramebufferTexture1D(GL_FRAMEBUFFER, attachmentType, textureTarget, textureHandle, mipLevel));
				break;
	#else
				throw std::runtime_error("1D textures are not supported by WebGL");
	#endif
			case GLInternalTextureFormat::Texture1DArray:
				glCall(context.FramebufferTextureLayer(GL_FRAMEBUFFER, attachmentType, textureHandle, mipLevel, arrayLayer));
				break;
			case GLInternalTextureFormat::Texture2D:
			case GLInternalTextureFormat::Texture2DMultisample:
				glCall(context.FramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget, textureHandle, mipLevel));
				break;
			case GLInternalTextureFormat::Texture2DArray:
			case GLInternalTextureFormat::Texture2DArrayMultisample:
			case GLInternalTextureFormat::CubemapArray:
			case GLInternalTextureFormat::Texture3D:
				glCall(context.FramebufferTextureLayer(GL_FRAMEBUFFER, attachmentType, textureHandle, mipLevel, arrayLayer));
				break;
			case GLInternalTextureFormat::Cubemap:
				glCall(context.FramebufferTexture2D(GL_FRAMEBUFFER,
													GL_TEXTURE_CUBE_MAP_POSITIVE_X + arrayLayer,
													textureTarget,
													textureHandle,
													mipLevel));
				break;
			default: throw std::runtime_error("Could not find a valid texture format type");
		}
	}

	void AttachTexture(GLuint						framebuffer,
					   Ref<Graphics::TextureOpenGL> texture,
					   uint32_t						mipLevel,
					   uint32_t						arrayLayer,
					   Graphics::ImageAspect		aspect,
					   uint32_t						colourIndex,
					   const GladGLContext		   &context)
	{
		AttachTextureNonDSA(framebuffer, texture, mipLevel, arrayLayer, aspect, colourIndex, context);
	}

	void GetBaseType(const Graphics::VertexBufferElement &element,
					 GLenum								 &baseType,
					 uint32_t							 &componentCount,
					 GLboolean							 &normalized,
					 GLPrimitiveType					 &primitiveType)
	{
		switch (element.Type)
		{
			case Graphics::ShaderDataType::Byte:
				baseType	   = GL_BYTE;
				componentCount = 1;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Byte2:
				baseType	   = GL_BYTE;
				componentCount = 2;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Byte4:
				baseType	   = GL_BYTE;
				componentCount = 4;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::NormByte:
				baseType	   = GL_UNSIGNED_BYTE;
				componentCount = 1;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::NormByte2:
				baseType	   = GL_UNSIGNED_BYTE;
				componentCount = 2;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::NormByte4:
				baseType	   = GL_UNSIGNED_BYTE;
				componentCount = 4;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::Float:
				baseType	   = GL_FLOAT;
				componentCount = 1;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Float2:
				baseType	   = GL_FLOAT;
				componentCount = 2;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Float3:
				baseType	   = GL_FLOAT;
				componentCount = 3;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Float4:
				baseType	   = GL_FLOAT;
				componentCount = 4;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::Half:
				baseType	   = GL_HALF_FLOAT;
				componentCount = 1;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Half2:
				baseType	   = GL_HALF_FLOAT;
				componentCount = 2;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Half4:
				baseType	   = GL_HALF_FLOAT;
				componentCount = 4;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::Int:
				baseType	   = GL_INT;
				componentCount = 1;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;
			case Graphics::ShaderDataType::Int2:
				baseType	   = GL_INT;
				componentCount = 2;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;
			case Graphics::ShaderDataType::Int3:
				baseType	   = GL_INT;
				componentCount = 3;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;
			case Graphics::ShaderDataType::Int4:
				baseType	   = GL_INT;
				componentCount = 4;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;

			case Graphics::ShaderDataType::SignedByte:
				baseType	   = GL_BYTE;
				componentCount = 1;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::SignedByte2:
				baseType	   = GL_BYTE;
				componentCount = 2;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::SignedByte4:
				baseType	   = GL_BYTE;
				componentCount = 4;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::SignedByteNormalized:
				baseType	   = GL_BYTE;
				componentCount = 1;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::SignedByte2Normalized:
				baseType	   = GL_BYTE;
				componentCount = 2;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::SignedByte4Normalized:
				baseType	   = GL_BYTE;
				componentCount = 4;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::Short:
				baseType	   = GL_SHORT;
				componentCount = 1;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Short2:
				baseType	   = GL_SHORT;
				componentCount = 2;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Short4:
				baseType	   = GL_SHORT;
				componentCount = 4;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::ShortNormalized:
				baseType	   = GL_SHORT;
				componentCount = 1;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Short2Normalized:
				baseType	   = GL_SHORT;
				componentCount = 2;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::Short4Normalized:
				baseType	   = GL_SHORT;
				componentCount = 4;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::UInt:
				baseType	   = GL_UNSIGNED_INT;
				componentCount = 1;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;
			case Graphics::ShaderDataType::UInt2:
				baseType	   = GL_UNSIGNED_INT;
				componentCount = 2;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;
			case Graphics::ShaderDataType::UInt3:
				baseType	   = GL_UNSIGNED_INT;
				componentCount = 3;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;
			case Graphics::ShaderDataType::UInt4:
				baseType	   = GL_UNSIGNED_INT;
				componentCount = 4;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Int;
				break;

			case Graphics::ShaderDataType::UShort:
				baseType	   = GL_UNSIGNED_SHORT;
				componentCount = 1;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::UShort2:
				baseType	   = GL_UNSIGNED_SHORT;
				componentCount = 2;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::UShort4:
				baseType	   = GL_UNSIGNED_SHORT;
				componentCount = 4;
				normalized	   = false;
				primitiveType  = GLPrimitiveType::Float;
				break;

			case Graphics::ShaderDataType::UShortNormalized:
				baseType	   = GL_UNSIGNED_SHORT;
				componentCount = 1;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::UShort2Normalized:
				baseType	   = GL_UNSIGNED_SHORT;
				componentCount = 2;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			case Graphics::ShaderDataType::UShort4Normalized:
				baseType	   = GL_UNSIGNED_SHORT;
				componentCount = 4;
				normalized	   = true;
				primitiveType  = GLPrimitiveType::Float;
				break;
			default: throw std::runtime_error("Failed to find valid vertex buffer element type");
		}
	}

	GLenum GetGLImageAspect(Graphics::ImageAspect aspect)
	{
		switch (aspect)
		{
			case Graphics::ImageAspect::Colour: return GL_RGBA;
			case Graphics::ImageAspect::Depth: return GL_DEPTH_COMPONENT;
			case Graphics::ImageAspect::Stencil: return GL_STENCIL_INDEX;
			case Graphics::ImageAspect::DepthStencil: return GL_DEPTH_STENCIL;
			default: throw std::runtime_error("Invalid aspect mask specified");
		}
	}

	GLenum GetAttachmentType(Graphics::ImageAspect aspect, uint32_t index)
	{
		switch (aspect)
		{
			case Graphics::ImageAspect::Colour: return GL_COLOR_ATTACHMENT0 + index;
			case Graphics::ImageAspect::Depth: return GL_DEPTH_ATTACHMENT;
			case Graphics::ImageAspect::Stencil: return GL_STENCIL_ATTACHMENT;
			case Graphics::ImageAspect::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
			default: throw std::runtime_error("Invalid aspect mask specified");
		}
	}

	GLenum GetBufferMaskToCopy(Graphics::ImageAspect aspect)
	{
		switch (aspect)
		{
			case Graphics::ImageAspect::Colour: return GL_COLOR_ATTACHMENT0;
			case Graphics::ImageAspect::Depth: return GL_DEPTH_ATTACHMENT;
			case Graphics::ImageAspect::Stencil: return GL_STENCIL_ATTACHMENT;
			case Graphics::ImageAspect::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
			default: throw std::runtime_error("Invalid aspect mask specified");
		}
	}

	std::vector<GLenum> GetWebGLBufferTargets(uint16_t usage)
	{
		std::vector<GLenum> targets;

		if (usage & Graphics::BufferUsage::Vertex)
		{
			targets.push_back(GL_ARRAY_BUFFER);
		}

		if (usage & Graphics::BufferUsage::Index)
		{
			targets.push_back(GL_ELEMENT_ARRAY_BUFFER);
		}

		if (usage & Graphics::BufferUsage::Uniform)
		{
			targets.push_back(GL_UNIFORM_BUFFER);
		}

		// default to GL_COPY_READ_BUFFER if no other targets are provided
		if (targets.size() == 0)
		{
			targets.push_back(GL_COPY_READ_BUFFER);
		}

		return targets;
	}

	std::unique_ptr<IViewContext> CreateViewContext(IWindow *window, Graphics::GraphicsDevice *device)
	{
		GL::ContextSpecification spec = {};
		spec.Debug					  = true;
		spec.Samples				  = 8;
		spec.GLVersion				  = GL::OpenGLVersion::OpenGL;

		Graphics::GraphicsDeviceOpenGL *deviceOpenGL = (Graphics::GraphicsDeviceOpenGL *)device;
		NativeWindowInfo				windowInfo	 = window->GetNativeWindowInfo();

	#if defined(NX_PLATFORM_WGL)
		OffscreenContextWGL *pbufferWGL = (OffscreenContextWGL *)deviceOpenGL->GetOffscreenContext();
		return std::make_unique<ViewContextWGL>(windowInfo.hwnd, windowInfo.hdc, pbufferWGL, spec);
	#elif defined(NX_PLATFORM_EGL)
		#if defined(NX_PLATFORM_ANDROID)
		spec.GLVersion	  = GL::OpenGLVersion::OpenGLES;
		spec.VersionMajor = 3;
		spec.VersionMinor = 2;
		#endif

		OffscreenContextEGL *pbufferEGL = (OffscreenContextEGL *)deviceOpenGL->GetOffscreenContext();

		#if defined(NX_PLATFORM_ANDROID)
		EGLDisplay			display		 = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		EGLNativeWindowType nativeWindow = (EGLNativeWindowType)windowInfo.nativeWindow;
		return std::make_unique<ViewContextEGL>(display, nativeWindow, pbufferEGL, spec);
		#else
		EGLDisplay			display		 = eglGetDisplay(windowInfo.display);
		EGLNativeWindowType nativeWindow = (EGLNativeWindowType)windowInfo.window;
		return std::make_unique<ViewContextEGL>(display, nativeWindow, pbufferEGL, spec);
		#endif
	#elif defined(NX_PLATFORM_WEBGL)
		return std::make_unique<ViewContextWebGL>(windowInfo.canvasId.c_str(), deviceOpenGL, spec);
	#else
		#error No OpenGL backend available
	#endif
	}

	void CopyBufferToTextureDSA(Ref<Graphics::TextureOpenGL>	  texture,
								Ref<Graphics::DeviceBufferOpenGL> buffer,
								uint32_t						  bufferOffset,
								Graphics::SubresourceDescription  subresource,
								const GladGLContext				 &context)
	{
		NX_ASSERT(texture->GetSpecification().Samples == 1, "Cannot set data in a multisampled texture");

		if (subresource.Depth > 1)
		{
			NX_ASSERT(texture->GetSpecification().Type == Graphics::TextureType::Texture3D,
					  "Attempting to set data in a multi-layer texture, but texture is not multi layer");
		}

		GLenum dataFormat = texture->GetDataFormat();
		GLenum baseType	  = texture->GetBaseType();

		context.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->GetHandle());

		GLenum	 glAspect	= GL::GetGLImageAspect(subresource.Aspect);
		uint32_t bufferSize = (subresource.Width - subresource.X) * (subresource.Height - subresource.Y) *
							  (uint32_t)GetPixelFormatSizeInBytes(texture->GetSpecification().Format);

		switch (texture->GetInternalGLTextureFormat())
		{
			case GL::GLInternalTextureFormat::Texture1D:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TextureSubImage1D(texture->GetHandle(),
												 subresource.MipLevel,
												 subresource.X,
												 subresource.Width,
												 dataFormat,
												 baseType,
												 (const void *)(uint64_t)bufferOffset));
				break;
	#else
				throw std::runtime_error("1D textures are not supported in WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Texture2DMultisample:
				glCall(context.TextureSubImage2D(texture->GetHandle(),
												 subresource.MipLevel,
												 subresource.X,
												 subresource.Y,
												 subresource.Width,
												 subresource.Height,
												 dataFormat,
												 baseType,
												 (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Cubemap:
			case GL::GLInternalTextureFormat::Texture2DArray:
			case GL::GLInternalTextureFormat::CubemapArray:
			case GL::GLInternalTextureFormat::Texture3D:
			case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
				glCall(context.TextureSubImage3D(texture->GetHandle(),
												 subresource.MipLevel,
												 subresource.X,
												 subresource.Y,
												 subresource.ArrayLayer,
												 subresource.Width,
												 subresource.Height,
												 subresource.Depth,
												 dataFormat,
												 baseType,
												 (const void *)(uint64_t)bufferOffset));
				break;
		}

		context.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	void CopyBufferToTextureNonDSA(Ref<Graphics::TextureOpenGL>		 texture,
								   Ref<Graphics::DeviceBufferOpenGL> buffer,
								   uint32_t							 bufferOffset,
								   Graphics::SubresourceDescription	 subresource,
								   const GladGLContext				&context)
	{
		NX_ASSERT(texture->GetSpecification().Samples == 1, "Cannot set data in a multisampled texture");

		if (subresource.Depth > 1)
		{
			NX_ASSERT(texture->GetSpecification().Type == Graphics::TextureType::Texture3D,
					  "Attempting to set data in a multi-layer texture, but texture is not multi layer");
		}

		GLenum textureType = texture->GetTextureType();
		GLenum dataFormat  = texture->GetDataFormat();
		GLenum baseType	   = texture->GetBaseType();

		glCall(context.BindTexture(textureType, texture->GetHandle()));
		context.BindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer->GetHandle());

		GLenum	 glAspect	= GL::GetGLImageAspect(subresource.Aspect);
		uint32_t bufferSize = (subresource.Width - subresource.X) * (subresource.Height - subresource.Y) *
							  (uint32_t)GetPixelFormatSizeInBytes(texture->GetSpecification().Format);

		switch (texture->GetInternalGLTextureFormat())
		{
			case GL::GLInternalTextureFormat::Texture1D:
	#if !defined(__EMSCRIPTEN__)
				glCall(context.TexSubImage1D(textureType,
											 subresource.MipLevel,
											 subresource.X,
											 subresource.Width,
											 dataFormat,
											 baseType,
											 (const void *)(uint64_t)bufferOffset));
				break;
	#else
				throw std::runtime_error("1D textures are not supported in WebGL");
	#endif
			case GL::GLInternalTextureFormat::Texture1DArray:
			case GL::GLInternalTextureFormat::Texture2D:
			case GL::GLInternalTextureFormat::Texture2DMultisample:
				glCall(context.TexSubImage2D(textureType,
											 subresource.MipLevel,
											 subresource.X,
											 subresource.Y,
											 subresource.Width,
											 subresource.Height,
											 dataFormat,
											 baseType,
											 (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Cubemap:
				glCall(context.TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + subresource.ArrayLayer,
											 subresource.MipLevel,
											 subresource.X,
											 subresource.Y,
											 subresource.Width,
											 subresource.Height,
											 dataFormat,
											 baseType,
											 (const void *)(uint64_t)bufferOffset));
				break;
			case GL::GLInternalTextureFormat::Texture2DArray:
			case GL::GLInternalTextureFormat::CubemapArray:
			case GL::GLInternalTextureFormat::Texture3D:
			case GL::GLInternalTextureFormat::Texture2DArrayMultisample:
				glCall(context.TexSubImage3D(textureType,
											 subresource.MipLevel,
											 subresource.X,
											 subresource.Y,
											 subresource.ArrayLayer,
											 subresource.Width,
											 subresource.Height,
											 subresource.Depth,
											 dataFormat,
											 baseType,
											 (const void *)(uint64_t)bufferOffset));
				break;
		}

		context.BindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		glCall(context.BindTexture(textureType, 0));
	}

	void CopyBufferToTexture(Ref<Graphics::TextureOpenGL>	   texture,
							 Ref<Graphics::DeviceBufferOpenGL> buffer,
							 uint32_t						   bufferOffset,
							 Graphics::SubresourceDescription  subresource,
							 const GladGLContext			  &context)
	{
		if (context.ARB_direct_state_access || context.EXT_direct_state_access)
		{
			CopyBufferToTextureDSA(texture, buffer, bufferOffset, subresource, context);
		}
		else
		{
			CopyBufferToTextureNonDSA(texture, buffer, bufferOffset, subresource, context);
		}
	}

	void CopyTextureToBufferDSA(Ref<Graphics::TextureOpenGL>	  texture,
								Ref<Graphics::DeviceBufferOpenGL> buffer,
								uint32_t						  bufferOffset,
								Graphics::SubresourceDescription  subresource,
								const GladGLContext				 &context)
	{
		const auto &textureSpecification = texture->GetSpecification();

		size_t layerSize =
			(subresource.Width - subresource.X) * (subresource.Height - subresource.Y) * GetPixelFormatSizeInBytes(textureSpecification.Format);
		size_t bufferSize = layerSize * subresource.Depth;
		GLenum glAspect	  = GL::GetGLImageAspect(subresource.Aspect);

		GLenum dataFormat = texture->GetDataFormat();
		GLenum baseType	  = texture->GetBaseType();

		context.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->GetHandle());

		for (uint32_t layer = subresource.Z; layer < subresource.Depth; layer++)
		{
			GLuint framebufferHandle = 0;
			glCall(context.CreateFramebuffers(1, &framebufferHandle));
			GL::AttachTexture(framebufferHandle, texture, subresource.MipLevel, layer, subresource.Aspect, 0, context);

			if (context.CheckNamedFramebufferStatus(framebufferHandle, GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				throw std::runtime_error("Framebuffer incomplete during texture read");
			}

			glCall(context.ReadBuffer(GL_COLOR_ATTACHMENT0));
			glCall(context.ReadPixels(subresource.X,
									  subresource.Y,
									  subresource.Width,
									  subresource.Height,
									  dataFormat,
									  baseType,
									  (void *)(uint64_t)bufferOffset));

			glCall(context.DeleteFramebuffers(1, &framebufferHandle));
			bufferOffset += layerSize;
		}

		glCall(context.BindBuffer(GL_PIXEL_PACK_BUFFER, 0));
	}

	void CopyTextureToBufferNonDSA(Ref<Graphics::TextureOpenGL>		 texture,
								   Ref<Graphics::DeviceBufferOpenGL> buffer,
								   uint32_t							 bufferOffset,
								   Graphics::SubresourceDescription	 subresource,
								   const GladGLContext				&context)
	{
		const auto &textureSpecification = texture->GetSpecification();

		size_t layerSize =
			(subresource.Width - subresource.X) * (subresource.Height - subresource.Y) * GetPixelFormatSizeInBytes(textureSpecification.Format);
		size_t bufferSize = layerSize * subresource.Depth;
		GLenum glAspect	  = GL::GetGLImageAspect(subresource.Aspect);

		GLenum textureType = texture->GetTextureType();
		GLenum dataFormat  = texture->GetDataFormat();
		GLenum baseType	   = texture->GetBaseType();

		context.BindBuffer(GL_PIXEL_PACK_BUFFER, buffer->GetHandle());

		for (uint32_t layer = subresource.Z; layer < subresource.Depth; layer++)
		{
			GLuint framebufferHandle = 0;
			glCall(context.GenFramebuffers(1, &framebufferHandle));
			glCall(context.BindFramebuffer(GL_FRAMEBUFFER, framebufferHandle));
			GL::AttachTexture(framebufferHandle, texture, subresource.MipLevel, layer, subresource.Aspect, 0, context);

			GL::ValidateFramebuffer(framebufferHandle, context);

			glCall(context.ReadBuffer(GL_COLOR_ATTACHMENT0));
			glCall(context.ReadPixels(subresource.X,
									  subresource.Y,
									  subresource.Width,
									  subresource.Height,
									  dataFormat,
									  baseType,
									  (void *)(uint64_t)bufferOffset));

			glCall(context.Flush());
			glCall(context.Finish());

			glCall(context.DeleteFramebuffers(1, &framebufferHandle));
			bufferOffset += layerSize;
		}

		glCall(context.BindTexture(textureType, 0));
		glCall(context.BindBuffer(GL_PIXEL_PACK_BUFFER, 0));
	}

	void CopyTextureToBuffer(Ref<Graphics::TextureOpenGL>	   texture,
							 Ref<Graphics::DeviceBufferOpenGL> buffer,
							 uint32_t						   bufferOffset,
							 Graphics::SubresourceDescription  subresource,
							 const GladGLContext			  &context)
	{
		if (context.ARB_direct_state_access || context.EXT_direct_state_access)
		{
			CopyTextureToBufferDSA(texture, buffer, bufferOffset, subresource, context);
		}
		else
		{
			CopyTextureToBufferNonDSA(texture, buffer, bufferOffset, subresource, context);
		}
	}

	void CopyTextureToTextureDSA(Ref<Graphics::TextureOpenGL>			 source,
								 Ref<Graphics::TextureOpenGL>			 destination,
								 const Graphics::TextureCopyDescription &copyDesc,
								 const GladGLContext					&context)
	{
		GLenum srcGlAspect		 = GL::GetGLImageAspect(copyDesc.SourceSubresource.Aspect);
		GLenum srcAttachmentType = GL::GetAttachmentType(copyDesc.SourceSubresource.Aspect, 0);
		GLenum dstGlAspect		 = GL::GetGLImageAspect(copyDesc.DestinationSubresource.Aspect);
		GLenum dstAttachmentType = GL::GetAttachmentType(copyDesc.DestinationSubresource.Aspect, 0);

		for (uint32_t layer = copyDesc.SourceSubresource.Z; layer < copyDesc.SourceSubresource.Depth; layer++)
		{
			GLuint sourceFramebufferHandle = 0;
			GLuint destFramebufferHandle   = 0;

			// set up source framebuffer
			{
				context.CreateFramebuffers(1, &sourceFramebufferHandle);
				GLenum aspectMask = GL::GetGLImageAspect(copyDesc.SourceSubresource.Aspect);
				GL::AttachTexture(sourceFramebufferHandle,
								  source,
								  copyDesc.SourceSubresource.MipLevel,
								  layer,
								  copyDesc.SourceSubresource.Aspect,
								  0,
								  context);
				GL::ValidateFramebuffer(sourceFramebufferHandle, context);
			}

			// set up dest framebuffer
			{
				context.CreateFramebuffers(1, &destFramebufferHandle);
				GLenum aspectMask = GL::GetGLImageAspect(copyDesc.DestinationSubresource.Aspect);
				GL::AttachTexture(destFramebufferHandle,
								  destination,
								  copyDesc.DestinationSubresource.MipLevel,
								  layer,
								  copyDesc.DestinationSubresource.Aspect,
								  0,
								  context);
				GL::ValidateFramebuffer(destFramebufferHandle, context);
			}

			context.BlitNamedFramebuffer(sourceFramebufferHandle,
										 destFramebufferHandle,
										 copyDesc.SourceSubresource.X,
										 copyDesc.SourceSubresource.Y,
										 copyDesc.SourceSubresource.Width,
										 copyDesc.SourceSubresource.Height,
										 copyDesc.DestinationSubresource.X,
										 copyDesc.DestinationSubresource.Y,
										 copyDesc.DestinationSubresource.Width,
										 copyDesc.DestinationSubresource.Height,
										 GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
										 GL_NEAREST);

			std::array<uint32_t, 2> framebuffers = {sourceFramebufferHandle, destFramebufferHandle};
			context.DeleteFramebuffers(framebuffers.size(), framebuffers.data());
		}
	}

	void CopyTextureToTextureNonDSA(Ref<Graphics::TextureOpenGL>			source,
									Ref<Graphics::TextureOpenGL>			destination,
									const Graphics::TextureCopyDescription &copyDesc,
									const GladGLContext					   &context)
	{
		GLenum srcGlAspect		 = GL::GetGLImageAspect(copyDesc.SourceSubresource.Aspect);
		GLenum srcAttachmentType = GL::GetAttachmentType(copyDesc.SourceSubresource.Aspect, 0);
		GLenum dstGlAspect		 = GL::GetGLImageAspect(copyDesc.DestinationSubresource.Aspect);
		GLenum dstAttachmentType = GL::GetAttachmentType(copyDesc.DestinationSubresource.Aspect, 0);

		for (uint32_t layer = copyDesc.SourceSubresource.Z; layer < copyDesc.SourceSubresource.Depth; layer++)
		{
			GLuint sourceFramebufferHandle = 0;
			GLuint destFramebufferHandle   = 0;

			// set up source framebuffer
			{
				glCall(context.GenFramebuffers(1, &sourceFramebufferHandle));
				glCall(context.BindFramebuffer(GL_FRAMEBUFFER, sourceFramebufferHandle));
				GLenum aspectMask = GL::GetGLImageAspect(copyDesc.SourceSubresource.Aspect);
				GL::AttachTexture(sourceFramebufferHandle,
								  source,
								  copyDesc.SourceSubresource.MipLevel,
								  layer,
								  copyDesc.SourceSubresource.Aspect,
								  0,
								  context);
				GL::ValidateFramebuffer(sourceFramebufferHandle, context);
			}

			// set up dest framebuffer
			{
				glCall(context.GenFramebuffers(1, &destFramebufferHandle));
				glCall(context.BindFramebuffer(GL_FRAMEBUFFER, destFramebufferHandle));
				GLenum aspectMask = GL::GetGLImageAspect(copyDesc.DestinationSubresource.Aspect);
				GL::AttachTexture(destFramebufferHandle,
								  destination,
								  copyDesc.DestinationSubresource.MipLevel,
								  layer,
								  copyDesc.DestinationSubresource.Aspect,
								  0,
								  context);
				GL::ValidateFramebuffer(destFramebufferHandle, context);
			}

			context.BindFramebuffer(GL_READ_FRAMEBUFFER, sourceFramebufferHandle);
			context.BindFramebuffer(GL_DRAW_FRAMEBUFFER, destFramebufferHandle);

			// copy all attached aspect masks
			context.BlitFramebuffer(copyDesc.SourceSubresource.X,
									copyDesc.SourceSubresource.Y,
									copyDesc.SourceSubresource.Width,
									copyDesc.SourceSubresource.Height,
									copyDesc.DestinationSubresource.X,
									copyDesc.DestinationSubresource.Y,
									copyDesc.DestinationSubresource.Width,
									copyDesc.DestinationSubresource.Height,
									GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
									GL_NEAREST);

			context.BindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			context.BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glCall(context.DeleteFramebuffers(1, &sourceFramebufferHandle));
			glCall(context.DeleteFramebuffers(1, &destFramebufferHandle));
		}
	}

	void CopyTextureToTexture(Ref<Graphics::TextureOpenGL>			  source,
							  Ref<Graphics::TextureOpenGL>			  destination,
							  const Graphics::TextureCopyDescription &copyDesc,
							  const GladGLContext					 &context)
	{
		if (context.ARB_direct_state_access || context.EXT_direct_state_access)
		{
			CopyTextureToTextureDSA(source, destination, copyDesc, context);
		}
		else
		{
			CopyTextureToTextureNonDSA(source, destination, copyDesc, context);
		}
	}

	static IGLContext *s_Context = nullptr;

	IGLContext *GetCurrentContext()
	{
		return s_Context;
	}

	void SetCurrentContext(IGLContext *context)
	{
		s_Context = context;
		s_Context->MakeCurrent();
	}

	void ClearCurrentContext()
	{
		s_Context = nullptr;
	}

	void ExecuteGLCommands(std::function<void(const GladGLContext &context)> function)
	{
		if (s_Context)
		{
			const GladGLContext &gladContext = s_Context->GetContext();
			function(gladContext);
		}
	}

	GLenum GetSamplerState(Nexus::Graphics::SamplerState state)
	{
		switch (state)
		{
			case Nexus::Graphics::SamplerState::LinearClamp:
			case Nexus::Graphics::SamplerState::LinearWrap: return GL_LINEAR;
			case Nexus::Graphics::SamplerState::PointClamp:
			case Nexus::Graphics::SamplerState::PointWrap: return GL_NEAREST;
			default: throw std::runtime_error("Invalid sampler state selected");
		}
	}

	GLenum GetWrapMode(Nexus::Graphics::SamplerState state)
	{
		switch (state)
		{
			case Nexus::Graphics::SamplerState::LinearClamp:
			case Nexus::Graphics::SamplerState::PointClamp: return GL_CLAMP_TO_EDGE;
			case Nexus::Graphics::SamplerState::LinearWrap:
			case Nexus::Graphics::SamplerState::PointWrap: return GL_REPEAT;
			default: throw std::runtime_error("Invalid sampler state selected");
		}
	}
}	 // namespace Nexus::GL

#endif