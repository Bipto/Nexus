#pragma once

#include "Core/Graphics/GraphicsDevice.hpp"
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
        void SetFramebuffer(Ref<Framebuffer> framebuffer);
        virtual void SubmitCommandList(Ref<CommandList> commandList) override;

        virtual void SetViewport(const Viewport &viewport) override;

        virtual const std::string GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Ref<Texture> CreateTexture(const TextureSpecification &spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(Ref<RenderPass> renderPass) override;
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
        virtual Ref<CommandList> CreateCommandList();

        virtual Ref<VertexBuffer> CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const BufferDescription &description, const void *data) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferDescription &description, const void *data) override;

        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification) override;
        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain) override;

        virtual void Resize(Point<int> size) override;

        virtual ShaderLanguage GetSupportedShaderFormat() override;
        virtual float GetUVCorrection() { return 1.0f; }

        virtual Swapchain *GetSwapchain() override;

    private:
        SDL_GLContext m_Context;
        const char *m_GlslVersion;
        Ref<FramebufferOpenGL> m_BoundFramebuffer = nullptr;
        VSyncState m_VsyncState = VSyncState::Enabled;
        SwapchainOpenGL *m_Swapchain = nullptr;
    };
}