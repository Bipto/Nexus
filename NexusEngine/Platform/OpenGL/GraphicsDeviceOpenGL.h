#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"
#include "FramebufferOpenGL.h"

namespace Nexus
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
    public:
        GraphicsDeviceOpenGL(const GraphicsDeviceCreateInfo &createInfo);
        GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL &) = delete;
        virtual ~GraphicsDeviceOpenGL();
        void SetContext() override;
        virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) override;
        virtual void SubmitCommandList(Ref<CommandList> commandList) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual const Viewport &GetViewport() override;

        virtual const char *GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<Vertex> vertices) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const UniformResourceBinding &binding) override;
        virtual Ref<Texture> CreateTexture(TextureSpecification spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification &spec) override;
        virtual Ref<Pipeline> CreatePipeline(const PipelineDescription &description) override;
        virtual Ref<CommandList> CreateCommandList() override;

        virtual void InitialiseImGui() override;
        virtual void BeginImGuiRender() override;
        virtual void EndImGuiRender() override;

        virtual void Resize(Point<int> size) override;
        virtual void SwapBuffers() override;
        virtual void SetVSyncState(VSyncState vSyncState) override;
        virtual VSyncState GetVsyncState() override;

        virtual ShaderLanguage GetSupportedShaderFormat() override { return ShaderLanguage::GLSL; }
        virtual float GetUVCorrection() { return 1.0f; }

    private:
        SDL_GLContext m_Context;
        const char *m_GlslVersion;
        Ref<FramebufferOpenGL> m_BoundFramebuffer = nullptr;
        VSyncState m_VsyncState = VSyncState::Enabled;
    };
}