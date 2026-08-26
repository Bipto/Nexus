#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/nxpch.hpp"

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <emscripten/html5.h>
#include <webgl/webgl2.h>
#endif

#include "Platform/OpenGL/OpenGLInclude.hpp"

#include "Context/IViewContext.hpp"
#include "Nexus-Core/Vertex.hpp"
#include "Platform/IWindow.hpp"
#include "Platform/Logging/Log.hpp"
#include "RHI/CommandList.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/IPhysicalDevice.hpp"
#include "RHI/ShaderModule.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::Graphics
{
    class TextureOpenGL;
    class DeviceBufferOpenGL;
} // namespace Nexus::Graphics

namespace Nexus::GL
{
    class IOffscreenContext;

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
    void GetSamplerFilter(Nexus::Graphics::SamplerFilter filter, GLenum &min, GLenum &max, bool hasMipmaps);
    GLenum GetPixelType(Nexus::Graphics::PixelFormat format);
    GLenum GetPixelDataFormat(Nexus::Graphics::PixelFormat format);
    GLenum GetSizedInternalFormat(Nexus::Graphics::PixelFormat format);

    GLenum GetGLIndexBufferFormat(Nexus::Graphics::IndexFormat format);
    GLenum GetTopology(Nexus::Graphics::Topology topology);

    GLenum GetShaderStage(Nexus::Graphics::ShaderStage stage);

    GLenum GetBufferUsage(const Graphics::DeviceBufferDescription &desc);

    GLenum GetAccessMask(Graphics::ShaderAccess access);
    GLenum GetTextureType(const Graphics::TextureDescription &spec);
    GLenum GetViewType(const Graphics::TextureViewDescription &desc);

    GLbitfield GetBarrierFlags(Graphics::BarrierAccess access, bool supportsStorageBuffers, bool &supportsByRegion);

    GLInternalTextureFormat GetGLInternalTextureFormat(const Graphics::TextureDescription &spec);
    void ValidateFramebuffer(GLuint framebuffer, GL::IGLContext *context);
    void AttachTexture(GLuint framebuffer, const Graphics::FramebufferTextureDescription &desc, bool isDepth,
                       uint32_t colourIndex, GL::IGLContext *context);

    void GetBaseType(const Graphics::VertexBufferElement &element, GLenum &baseType, uint32_t &componentCount,
                     GLboolean &normalized, GLPrimitiveType &primitiveType);

    GLenum GetGLImageAspect(bool isDepth);
    GLenum GetAttachmentType(bool isDepth, uint32_t index);

    std::vector<GLenum> GetWebGLBufferTargets(uint16_t usage);

    std::unique_ptr<IViewContext> CreateViewContext(IWindow *window, Graphics::IGraphicsDevice *device);

    void CopyBufferToTexture(const Graphics::CopyBufferToTextureCommand &command, GL::IGLContext *context);

    void CopyTextureToBuffer(const Graphics::CopyTextureToBufferCommand &command, GL::IGLContext *context);

    void CopyTextureToTexture(const Graphics::TextureCopyDescription &copyDesc, GL::IGLContext *context);

    /// @brief Function that loads required OpenGL functions,
    // this function should be called by IGraphicsAPI
    /// @return A boolean indicating whether OpenGL was able to be initialized
    bool LoadOpenGL();
    std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> LoadAvailablePhysicalDevices();
    std::vector<std::shared_ptr<Graphics::IPhysicalDevice>> LoadAvailablePhysicalDevices(bool debug);

} // namespace Nexus::GL

#endif

#define glClearErrors()                                                                                                \
    while (glGetError() != GL_NO_ERROR)                                                                                \
    {                                                                                                                  \
    }

#define glCheckErrors()                                                                                                \
    while (GLenum error = glGetError())                                                                                \
    {                                                                                                                  \
        std::string message = Nexus::GL::GetErrorMessageFromCode(error);                                               \
        NX_ERROR(message);                                                                                             \
    }

#if defined(NX_GL_DEBUG)
#define glCall(x)                                                                                                      \
    glClearErrors();                                                                                                   \
    x;                                                                                                                 \
    glCheckErrors();
#else
#define glCall(x) x;
#endif