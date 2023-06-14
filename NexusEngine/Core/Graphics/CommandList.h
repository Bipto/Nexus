#pragma once

#include "Color.h"
#include "Pipeline.h"
#include "Buffer.h"

#include <functional>

namespace Nexus
{
    struct ClearValue
    {
        float Red = 1.0f;
        float Green = 1.0f;
        float Blue = 1.0f;
        float Alpha = 1.0f;
    };

    struct CommandListBeginInfo
    {
        ClearValue ClearValue;
        float DepthValue = 1.0f;
    };

    struct DrawElementCommand
    {
        uint32_t Start = 0;
        uint32_t Count = 0;
    };

    struct DrawIndexedCommand
    {
        uint32_t Count = 0;
        uint32_t Offset = 0;
    };

    class CommandList
    {
    public:
        CommandList(Ref<Pipeline> pipeline)
            : m_Pipeline(pipeline) {}

        virtual void Begin(const CommandListBeginInfo &beginInfo) = 0;
        virtual void End() = 0;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) = 0;

        virtual void DrawElements(uint32_t start, uint32_t count) = 0;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) = 0;

        Ref<Pipeline> GetPipeline() const { return m_Pipeline; }

        virtual const ClearValue &GetClearColorValue() = 0;
        virtual const float GetClearDepthValue() = 0;

        virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() = 0;
        virtual const std::vector<Ref<IndexBuffer>> &GetIndexBuffers() = 0;

        virtual Ref<VertexBuffer> GetCurrentVertexBuffer() = 0;
        virtual Ref<IndexBuffer> GetCurrentIndexBuffer() = 0;
        virtual DrawElementCommand &GetCurrentDrawElementCommand() = 0;
        virtual DrawIndexedCommand &GetCurrentDrawIndexedCommand() = 0;

    protected:
        Ref<Pipeline> m_Pipeline = nullptr;
    };

    typedef void (*RenderCommand)(Ref<CommandList> commandList);
}