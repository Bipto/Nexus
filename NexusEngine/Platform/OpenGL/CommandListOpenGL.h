#pragma once

#include "Core/Graphics/CommandList.h"

#include "GL.h"

#include <array>

namespace Nexus
{
    class CommandListOpenGL : public CommandList
    {
    public:
        CommandListOpenGL(Ref<Pipeline> pipeline);

        virtual void Begin(const CommandListBeginInfo &beginInfo) override;
        virtual void End() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual const ClearValue &GetClearColorValue() override;
        virtual const float GetClearDepthValue() override;

        virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() override { return m_VertexBuffers; }
        virtual const std::vector<Ref<IndexBuffer>> &GetIndexBuffers() override { return m_IndexBuffers; }

        virtual Ref<VertexBuffer> GetCurrentVertexBuffer() override;
        virtual Ref<IndexBuffer> GetCurrentIndexBuffer() override;
        virtual DrawElementCommand &GetCurrentDrawElementCommand() override;
        virtual DrawIndexedCommand &GetCurrentDrawIndexedCommand() override;

        GLenum GetTopology();
        const std::array<RenderCommand, 1000> &GetRenderCommands();
        uint32_t GetCommandCount();

    private:
        std::array<RenderCommand, 1000> m_Commands;
        uint32_t m_CommandIndex = 0;

        CommandListBeginInfo m_CommandListBeginInfo;

        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        std::vector<Ref<IndexBuffer>> m_IndexBuffers;
        std::vector<DrawElementCommand> m_ElementCommands;
        std::vector<DrawIndexedCommand> m_IndexedCommands;

        uint32_t m_VertexBufferIndex = 0;
        uint32_t m_IndexBufferIndex = 0;
        uint32_t m_ElementCommandIndex = 0;
        uint32_t m_IndexedCommandIndex = 0;
    };
}