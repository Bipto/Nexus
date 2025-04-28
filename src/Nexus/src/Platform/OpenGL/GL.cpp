#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"

	#include "Nexus-Core/Logging/Log.hpp"

	#if defined(NX_PLATFORM_WGL)
		#include "Context/WGL/OffscreenContextWGL.hpp"
		#include "Context/WGL/ViewContextWGL.hpp"
		#include "Context/WGL/PhysicalDeviceWGL.hpp"
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

	#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"
	#include "Nexus-Core/Platform.hpp"

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
			case Nexus::Graphics::BlendFactor::SourceColor: return GL_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusSourceColor: return GL_ONE_MINUS_SRC_COLOR;
			case Nexus::Graphics::BlendFactor::DestinationColor: return GL_DST_COLOR;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationColor: return GL_ONE_MINUS_DST_COLOR;
			case Nexus::Graphics::BlendFactor::SourceAlpha: return GL_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			case Nexus::Graphics::BlendFactor::DestinationAlpha: return GL_DST_ALPHA;
			case Nexus::Graphics::BlendFactor::OneMinusDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
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
			case Nexus::Graphics::SamplerAddressMode::Border: return GL_CLAMP_TO_BORDER_EXT;
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

			case Nexus::Graphics::PixelFormat::R16_UNorm: return depthFormat ? GL_DEPTH_COMPONENT16 : GL_R16_EXT;
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

			case Nexus::Graphics::PixelFormat::R16_G16_UNorm: return GL_RG16_EXT;
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

			default: throw std::runtime_error("Failed to find a valid format");
		}
	}

	GLenum GetGLIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format)
	{
		switch (format)
		{
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
	#if !defined(__EMSCRIPTEN__)
			case Nexus::Graphics::ShaderStage::Compute: return GL_COMPUTE_SHADER;
			case Nexus::Graphics::ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
			case Nexus::Graphics::ShaderStage::TesselationControl: return GL_TESS_CONTROL_SHADER;
			case Nexus::Graphics::ShaderStage::TesselationEvaluation: return GL_TESS_EVALUATION_SHADER;
	#endif
			case Nexus::Graphics::ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
			case Nexus::Graphics::ShaderStage::Vertex: return GL_VERTEX_SHADER;
			default: throw std::runtime_error("Failed to find a valid shader stage");
		}
	}

	GLenum GLCubemapFace(Nexus::Graphics::CubemapFace face)
	{
		switch (face)
		{
			case Nexus::Graphics::CubemapFace::PositiveX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			case Nexus::Graphics::CubemapFace::NegativeX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			case Nexus::Graphics::CubemapFace::PositiveY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			case Nexus::Graphics::CubemapFace::NegativeY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			case Nexus::Graphics::CubemapFace::PositiveZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			case Nexus::Graphics::CubemapFace::NegativeZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			default: throw std::runtime_error("Failed to find a valid cubemap face");
		}
	}

	GLenum GetBufferTarget(Graphics::DeviceBufferType type)
	{
		switch (type)
		{
			case Graphics::DeviceBufferType::Vertex: return GL_ARRAY_BUFFER;
			case Graphics::DeviceBufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
			case Graphics::DeviceBufferType::Uniform: return GL_UNIFORM_BUFFER;
			case Graphics::DeviceBufferType::Structured: return GL_SHADER_STORAGE_BUFFER;
			case Graphics::DeviceBufferType::Upload: return GL_COPY_READ_BUFFER;
			case Graphics::DeviceBufferType::Readback: return GL_COPY_WRITE_BUFFER;
			case Graphics::DeviceBufferType::Indirect: return GL_DRAW_INDIRECT_BUFFER;

			default: throw std::runtime_error("Failed to find valid buffer usage");
		}
	}

	GLenum GetBufferUsage(Graphics::DeviceBufferType type)
	{
		switch (type)
		{
			case Graphics::DeviceBufferType::Vertex:
			case Graphics::DeviceBufferType::Index:
			case Graphics::DeviceBufferType::Uniform:
			case Graphics::DeviceBufferType::Structured:
			case Graphics::DeviceBufferType::Indirect: return GL_DYNAMIC_DRAW;
			case Graphics::DeviceBufferType::Upload: return GL_DYNAMIC_COPY;
			case Graphics::DeviceBufferType::Readback: return GL_DYNAMIC_COPY;

			default: throw std::runtime_error("Failed to find valid buffer usage");
		}

		return GL_STATIC_DRAW;
	}

	GLenum GetAccessMask(Graphics::StorageImageAccess access)
	{
		switch (access)
		{
			case Graphics::StorageImageAccess::Read: return GL_READ_ONLY;
			case Graphics::StorageImageAccess::ReadWrite: return GL_READ_WRITE;
			default: throw std::runtime_error("Failed to find a valid access mask");
		}
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

	std::unique_ptr<IOffscreenContext> CreateOffscreenContext(Graphics::IPhysicalDevice *physicalDevice)
	{
		GL::ContextSpecification spec = {};
		spec.Debug					  = true;
		spec.Samples				  = 1;
		spec.GLVersion				  = GL::OpenGLVersion::OpenGL;

	#if defined(NX_PLATFORM_WGL)
		return std::make_unique<OffscreenContextWGL>(spec, physicalDevice);
	#elif defined(NX_PLATFORM_EGL)

		#if defined(NX_PLATFORM_ANDROID)
		spec.GLVersion	  = GL::OpenGLVersion::OpenGLES;
		spec.VersionMajor = 3;
		spec.VersionMinor = 2;
		#endif

		Nexus::WindowSpecification windowSpec {};
		windowSpec.Width	 = 1;
		windowSpec.Height	 = 1;
		windowSpec.Resizable = false;

		Nexus::Graphics::SwapchainSpecification swapchainSpec {};

		IWindow *window = Platform::CreatePlatformWindow(windowSpec);
		window->Hide();

		NativeWindowInfo windowInfo = window->GetNativeWindowInfo();

		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		#if !defined(NX_PLATFORM_ANDROID)
		display = eglGetDisplay(windowInfo.display);
		#endif
		return std::make_unique<OffscreenContextEGL>(display, spec);
	#elif defined(NX_PLATFORM_WEBGL)
		return std::make_unique<OffscreenContextWebGL>("offscreenContext");
	#elif defined(NX_PLATFORM_GLX)
		return std::make_unique<OffscreenContextGLX>(spec);
	#else
		#error No OpenGL backend available
	#endif
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