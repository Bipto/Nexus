#include "CommandList.hpp"

namespace Nexus::Graphics
{
CommandList::CommandList(const CommandListSpecification &spec) : m_Specification(spec)
{
}

    void CommandList::Begin()
    {
        m_CommandRecorder.Clear();
    }

    void CommandList::End()
    {
    }

    void CommandList::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
    {
        SetVertexBufferCommand command;
        command.VertexBufferRef = vertexBuffer;
        command.Slot = slot;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        m_CommandRecorder.PushCommand(indexBuffer);
    }

    void CommandList::SetPipeline(Ref<Pipeline> pipeline)
    {
        m_CommandRecorder.PushCommand(pipeline);
    }

    void CommandList::Draw(uint32_t start, uint32_t count)
    {
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        DrawIndexedCommand command;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
    {
        DrawInstancedCommand command;
        command.VertexCount = vertexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
    {
        DrawInstancedIndexedCommand command;
        command.IndexCount = indexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::SetResourceSet(Ref<ResourceSet> resources)
    {
        m_CommandRecorder.PushCommand(resources);
    }

    void CommandList::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        ClearColorTargetCommand command;
        command.Index = index;
        command.Color = color;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        ClearDepthStencilTargetCommand command;
        command.Value = value;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::SetRenderTarget(RenderTarget target)
    {
        m_CommandRecorder.PushCommand(target);
    }

    void CommandList::SetViewport(const Viewport &viewport)
    {
        m_CommandRecorder.PushCommand(viewport);
    }

    void CommandList::SetScissor(const Scissor &scissor)
    {
        m_CommandRecorder.PushCommand(scissor);
    }

    void CommandList::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
    {
        ResolveSamplesToSwapchainCommand command;
        command.Source = source;
        command.SourceIndex = sourceIndex;
        command.Target = target;
        m_CommandRecorder.PushCommand(command);
    }

    void Nexus::Graphics::CommandList::StartTimingQuery(Ref<TimingQuery> query)
    {
        StartTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    void Nexus::Graphics::CommandList::StopTimingQuery(Ref<TimingQuery> query)
    {
        StopTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandList::TransitionImageLayout(WeakRef<Texture> texture, uint32_t baseLevel, uint32_t numLevels, ImageLayout layout)
    {
        TransitionImageLayoutCommand command;
        command.TransitionTexture = texture;
        command.BaseLevel = baseLevel;
        command.NumLevels = numLevels;
        command.TextureLayout = layout;
        m_CommandRecorder.PushCommand(command);
    }

    CommandRecorder &CommandList::GetCommandRecorder()
    {
        return m_CommandRecorder;
    }

    const CommandListSpecification &CommandList::GetSpecification()
    {
        return m_Specification;
    }
}
