#pragma once

#include "Nexus/Graphics/CommandList.hpp"
#include "Nexus/Graphics/GraphicsDevice.hpp"

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

        virtual void BeginRenderPass(RenderPass *renderPass, const RenderPassBeginInfo &beginInfo) override;
        virtual void EndRenderPass() override;

        virtual void SetVertexBuffer(VertexBuffer *vertexBuffer) override;
        virtual void SetIndexBuffer(IndexBuffer *indexBuffer) override;
        virtual void SetPipeline(Pipeline *pipeline) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void UpdateUniformBuffer(UniformBuffer *buffer, void *data, uint32_t size, uint32_t offset) override;
        virtual void SetResourceSet(ResourceSet *resources) override;

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

        RenderPassBeginInfo m_CommandListBeginInfo;
        Pipeline *m_CurrentlyBoundPipeline;
    };
}