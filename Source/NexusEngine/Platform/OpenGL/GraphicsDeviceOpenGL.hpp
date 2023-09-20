#pragma once

#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "GL.hpp"
#include "FramebufferOpenGL.hpp"
#include "SwapchainOpenGL.hpp"
#include "SDL_opengl.h"

namespace Nexus::Graphics
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
    public:
        GraphicsDeviceOpenGL(const GraphicsDeviceCreateInfo &createInfo, Window *window);
        GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL &) = delete;
        virtual ~GraphicsDeviceOpenGL();
        void SetContext() override;
        void SetFramebuffer(Framebuffer *framebuffer);
        virtual void SubmitCommandList(CommandList *commandList) override;

        virtual const std::string GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual Shader *CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Texture *CreateTexture(const TextureSpecification &spec) override;
        virtual Framebuffer *CreateFramebuffer(RenderPass *renderPass) override;
        virtual Pipeline *CreatePipeline(const PipelineDescription &description) override;
        virtual CommandList *CreateCommandList();

        virtual VertexBuffer *CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) override;
        virtual IndexBuffer *CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format = IndexBufferFormat::UInt32) override;
        virtual UniformBuffer *CreateUniformBuffer(const BufferDescription &description, const void *data) override;

        virtual RenderPass *CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification) override;
        virtual RenderPass *CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain) override;
        virtual ResourceSet *CreateResourceSet(const ResourceSetSpecification &spec) override;

        virtual void Resize(Point<int> size) override;

        virtual ShaderLanguage GetSupportedShaderFormat() override;
        virtual float GetUVCorrection() { return 1.0f; }

        virtual Swapchain *GetSwapchain() override;

    private:
        SDL_GLContext m_Context;
        const char *m_GlslVersion;
        FramebufferOpenGL *m_BoundFramebuffer = nullptr;
        VSyncState m_VsyncState = VSyncState::Enabled;
        SwapchainOpenGL *m_Swapchain = nullptr;
    };
}