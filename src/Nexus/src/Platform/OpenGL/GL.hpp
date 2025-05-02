#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/nxpch.hpp"

	#if defined(__EMSCRIPTEN__)
		#include <emscripten.h>
		#include <emscripten/html5.h>
	#endif

	#if defined(__EMSCRIPTEN__) || defined(ANDROID)
		#include <GLES3/gl32.h>
		#include <GLES2/gl2ext.h>
	#elif defined(__ANDROID__)
		#include <GLES3/gl32.h>
		#include <glad/glad_egl.h>
	#else
		#include "Platform/Windows/WindowsInclude.hpp"
		#include "glad/glad.h"
	#endif

	#include "Nexus-Core/Graphics/ShaderModule.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"
	#include "Nexus-Core/Logging/Log.hpp"
	#include "Nexus-Core/Vertex.hpp"
	#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"

	#include "Nexus-Core/IWindow.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

	#include "Context/IOffscreenContext.hpp"
	#include "Context/IViewContext.hpp"

namespace Nexus::GL
{
	enum class GLPrimitiveType
	{
		Unknown = 0,
		Float,
		Int
	};

	enum class GLInternalTextureFormat
	{
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DMultisample,
		Texture2DArray,
		Texture2DArrayMultisample,
		Texture3D,
		Cubemap,
		CubemapArray
	};

	std::string GetErrorMessageFromCode(const GLenum error);

	GLenum GetStencilOperation(Nexus::Graphics::StencilOperation operation);
	GLenum GetComparisonFunction(Nexus::Graphics::ComparisonFunction function);
	GLenum GetBlendFactor(Nexus::Graphics::BlendFactor function);
	GLenum GetBlendFunction(Nexus::Graphics::BlendEquation equation);

	GLenum GetSamplerAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);
	void   GetSamplerFilter(Nexus::Graphics::SamplerFilter filter, GLenum &min, GLenum &max, bool hasMipmaps);
	GLenum GetPixelType(Nexus::Graphics::PixelFormat format);
	GLenum GetPixelDataFormat(Nexus::Graphics::PixelFormat format);
	GLenum GetSizedInternalFormat(Nexus::Graphics::PixelFormat format, bool depthFormat);

	GLenum GetGLIndexBufferFormat(Nexus::Graphics::IndexBufferFormat format);
	GLenum GetTopology(Nexus::Graphics::Topology topology);

	GLenum GetShaderStage(Nexus::Graphics::ShaderStage stage);
	GLenum GLCubemapFace(Nexus::Graphics::CubemapFace face);

	GLenum GetBufferTarget(Graphics::DeviceBufferType type);
	GLenum GetBufferUsage(Graphics::DeviceBufferType type);

	GLenum GetAccessMask(Graphics::StorageImageAccess access);
	GLenum					GetTextureType(const Graphics::TextureSpecification &spec);
	GLInternalTextureFormat GetGLInternalTextureFormat(const Graphics::TextureSpecification &spec);

	void GetBaseType(const Graphics::VertexBufferElement &element,
					 GLenum								 &baseType,
					 uint32_t							 &componentCount,
					 GLboolean							 &normalized,
					 GLPrimitiveType					 &primitiveType);

	std::unique_ptr<IOffscreenContext> CreateOffscreenContext(Graphics::IPhysicalDevice *physicalDevice);
	std::unique_ptr<IViewContext>	   CreateViewContext(IWindow *window, Graphics::GraphicsDevice *device);

	/// @brief Function that loads required OpenGL functions,
	// this function should be called by IGraphicsAPI
	/// @return A boolean indicating whether OpenGL was able to be initialized
	bool													LoadOpenGL();
	std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> LoadAvailablePhysicalDevices();

}	 // namespace Nexus::GL

#endif

#define glClearErrors()                                                                                                                              \
	while (glGetError() != GL_NO_ERROR) {}

#define glCheckErrors()                                                                                                                              \
	while (GLenum error = glGetError())                                                                                                              \
	{                                                                                                                                                \
		std::string message = Nexus::GL::GetErrorMessageFromCode(error);                                                                             \
		NX_ERROR(message);                                                                                                                           \
	}

#define NX_GL_DEBUG
#if defined(NX_GL_DEBUG)
	#define glCall(x)                                                                                                                                \
		glClearErrors();                                                                                                                             \
		x;                                                                                                                                           \
		glCheckErrors();
#else
	#define glCall(x) x;
#endif