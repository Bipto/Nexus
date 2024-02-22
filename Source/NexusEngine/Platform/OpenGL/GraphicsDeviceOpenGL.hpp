#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "GL.hpp"
#include "FramebufferOpenGL.hpp"
#include "SwapchainOpenGL.hpp"

namespace Nexus::Graphics
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
    public:
        GraphicsDeviceOpenGL(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec);
        GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL &) = delete;
        void SetContext() override;
        void SetFramebuffer(Framebuffer *framebuffer);
        void SetSwapchain(Swapchain *swapchain);
        virtual void SubmitCommandList(CommandList *commandList) override;

        virtual const std::string GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual Texture *CreateTexture(const TextureSpecification &spec) override;
        virtual Pipeline *CreatePipeline(const PipelineDescription &description) override;
        virtual CommandList *CreateCommandList();
        virtual VertexBuffer *CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) override;
        virtual IndexBuffer *CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format = IndexBufferFormat::UInt32) override;
        virtual UniformBuffer *CreateUniformBuffer(const BufferDescription &description, const void *data) override;
        virtual ResourceSet *CreateResourceSet(const ResourceSetSpecification &spec) override;
        virtual Framebuffer *CreateFramebuffer(const FramebufferSpecification &spec) override;
        virtual Sampler *CreateSampler(const SamplerSpecification &spec) override;

        virtual ShaderLanguage GetSupportedShaderFormat() override;
        virtual float GetUVCorrection() { return 1.0f; }

    private:
        virtual Shader *CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;

    private:
        const char *m_GlslVersion;
        FramebufferOpenGL *m_BoundFramebuffer = nullptr;
        VSyncState m_VsyncState = VSyncState::Enabled;
    };
}

#endif