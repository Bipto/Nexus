#pragma once

#if defined(NX_PLATFORM_OPENGL)

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Platform/OpenGL/BufferOpenGL.hpp"
#include "PipelineOpenGL.hpp"

#include "GL.hpp"

namespace Nexus::Graphics
{
    class CommandListOpenGL : public CommandList
    {
    public:
        CommandListOpenGL(GraphicsDevice *device);
        virtual ~CommandListOpenGL();

        virtual void Begin() override;
        virtual void End() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetPipeline(Ref<Pipeline> pipeline) override;

        virtual void Draw(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart) override;
        virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart) override;
        virtual void DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart) override;

        virtual void SetResourceSet(Ref<ResourceSet> resources) override;

        virtual void ClearColorTarget(uint32_t index, const ClearColorValue &color) override;
        virtual void ClearDepthTarget(const ClearDepthStencilValue &value) override;
        virtual void SetRenderTarget(RenderTarget target) override;

        virtual void SetViewport(const Viewport &viewport) override;
        virtual void SetScissor(const Scissor &scissor) override;
        virtual void ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target) override;

        virtual void StartTimingQuery(Ref<TimingQuery> query) override;
        virtual void StopTimingQuery(Ref<TimingQuery> query) override;

        CommandRecorder &GetCommandRecorder();

    private:
        GraphicsDevice *GetGraphicsDevice();

    private:
        GraphicsDevice *m_Device = nullptr;
        GLenum m_IndexBufferFormat;

        CommandRecorder m_CommandRecorder = {};
    };
}

#endif