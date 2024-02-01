#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus/Graphics/CommandList.hpp"
#include "Nexus/Graphics/GraphicsDevice.hpp"
#include "Platform/OpenGL/BufferOpenGL.hpp"

#include "GL.hpp"

#include <array>

namespace Nexus::Graphics
{
    class CommandListOpenGL : public CommandList
    {
    public:
        CommandListOpenGL(GraphicsDevice *device);

        virtual void Begin() override;
        virtual void End() override;

        virtual void SetVertexBuffer(VertexBuffer *vertexBuffer) override;
        virtual void SetIndexBuffer(IndexBuffer *indexBuffer) override;
        virtual void SetPipeline(Pipeline *pipeline) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart) override;

        virtual void SetResourceSet(ResourceSet *resources) override;

        virtual void ClearColorTarget(uint32_t index, const ClearColorValue &color) override;
        virtual void ClearDepthTarget(const ClearDepthStencilValue &value) override;
        virtual void SetRenderTarget(RenderTarget target) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual void SetScissor(const Scissor &scissor) override;
        virtual void ResolveFramebuffer(Framebuffer *source, uint32_t sourceIndex, Swapchain *target, uint32_t targetIndex) override;

    public:
        const std::vector<RenderCommand> &GetRenderCommands();
        RenderCommandData &GetCurrentCommandData();
        GLenum GetTopology();
        void BindPipeline(Pipeline *pipeline);
        GraphicsDevice *GetGraphicsDevice();
        GLenum m_IndexBufferFormat;

    private:
        GraphicsDevice *m_Device;
        std::vector<RenderCommand> m_Commands;
        std::vector<RenderCommandData> m_CommandData;
        uint32_t m_CommandIndex = 0;
        Pipeline *m_CurrentlyBoundPipeline;
        RenderTarget m_CurrentRenderTarget;
        Nexus::Graphics::VertexBufferOpenGL *m_CurrentlyBoundVertexBuffer = nullptr;
    };
}

#endif