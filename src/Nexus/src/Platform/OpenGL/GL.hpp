#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/nxpch.hpp"

	#if defined(__EMSCRIPTEN__)
		#include <emscripten.h>
		#include <emscripten/html5.h>
	#endif

	#if defined(__EMSCRIPTEN__) || defined(ANDROID) || defined(__ANDROID__)
		#include <GLES3/gl32.h>
		#include <GLES2/gl2ext.h>
	#else
		#include "Platform/Windows/WindowsInclude.hpp"
		#include "glad/glad.h"
	#endif

	#include "Nexus-Core/Graphics/ShaderModule.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"
	#include "Nexus-Core/Logging/Log.hpp"
	#include "Nexus-Core/Vertex.hpp"

	#include "Nexus-Core/IWindow.hpp"

	#include "Context/IOffscreenContext.hpp"
	#include "Context/IViewContext.hpp"

namespace Nexus::GL
{
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

	void GetBaseType(const Graphics::VertexBufferElement &element, GLenum &baseType, uint32_t &componentCount, GLboolean &normalized);

	std::unique_ptr<IOffscreenContext> CreateOffscreenContext();
	std::unique_ptr<IViewContext>	   CreateViewContext(IWindow *window, Graphics::GraphicsDevice *device);

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

#if defined(NX_GL_DEBUG)
	#define glCall(x)                                                                                                                                \
		glClearErrors();                                                                                                                             \
		x;                                                                                                                                           \
		glCheckErrors();
#else
	#define glCall(x) x;
#endif