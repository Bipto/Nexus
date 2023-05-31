#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
#include "TextureOpenGL.h"
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
        void Clear(float red, float green, float blue, float alpha) override;
        virtual void SetFramebuffer(Ref<Framebuffer> framebuffer) override;
        void DrawElements(PrimitiveType type, uint32_t start, uint32_t count) override;
        void DrawIndexed(PrimitiveType type, uint32_t count, uint32_t offset) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual const Viewport &GetViewport() override;

        virtual const char *GetAPIName() override;
        virtual const char *GetDeviceName() override;
        virtual void *GetContext() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetShader(Ref<Shader> shader) override;

        virtual Ref<Shader> CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout) override;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<Vertex> vertices) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) override;
        virtual Ref<UniformBuffer> CreateUniformBuffer(const UniformResourceBinding &binding) override;
        virtual Ref<Texture> CreateTexture(TextureSpecification spec) override;
        virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification &spec) override;

        virtual void InitialiseImGui() override;
        virtual void BeginImGuiRender() override;
        virtual void EndImGuiRender() override;

        virtual void Resize(Point<int> size) override;
        virtual void SwapBuffers() override;
        virtual void SetVSyncState(VSyncState vSyncState) override;
        virtual VSyncState GetVsyncState() override;

        virtual ShaderFormat GetSupportedShaderFormat() override { return ShaderFormat::GLSL; }

    private:
        SDL_GLContext m_Context;
        const char *m_GlslVersion;
        Ref<FramebufferOpenGL> m_BoundFramebuffer = nullptr;
        VSyncState m_VsyncState = VSyncState::Enabled;
    };
}