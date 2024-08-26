#include "CommandList.hpp"

namespace Nexus::Graphics
{
CommandList::CommandList(const CommandListSpecification &spec) : m_Specification(spec)
{
}

void CommandList::Begin()
{
    m_Commands.clear();
}

void CommandList::End()
{
}

void CommandList::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
{
    SetVertexBufferCommand command;
    command.VertexBufferRef = vertexBuffer;
    command.Slot = slot;
    m_Commands.push_back(command);
}

void CommandList::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
{
    m_Commands.push_back(indexBuffer);
}

void CommandList::SetPipeline(Ref<Pipeline> pipeline)
{
    m_Commands.push_back(pipeline);
}

void CommandList::Draw(uint32_t start, uint32_t count)
{
    DrawElementCommand command;
    command.Start = start;
    command.Count = count;
    m_Commands.push_back(command);
}

void CommandList::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
{
    DrawIndexedCommand command;
    command.VertexStart = vertexStart;
    command.IndexStart = indexStart;
    command.Count = count;
    m_Commands.push_back(command);
}

void CommandList::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
{
    DrawInstancedCommand command;
    command.VertexCount = vertexCount;
    command.InstanceCount = instanceCount;
    command.VertexStart = vertexStart;
    command.InstanceStart = instanceStart;
    m_Commands.push_back(command);
}

void CommandList::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
{
    DrawInstancedIndexedCommand command;
    command.IndexCount = indexCount;
    command.InstanceCount = instanceCount;
    command.VertexStart = vertexStart;
    command.IndexStart = indexStart;
    command.InstanceStart = instanceStart;
    m_Commands.push_back(command);
}

void CommandList::SetResourceSet(Ref<ResourceSet> resources)
{
    m_Commands.push_back(resources);
}

void CommandList::ClearColorTarget(uint32_t index, const ClearColorValue &color)
{
    ClearColorTargetCommand command;
    command.Index = index;
    command.Color = color;
    m_Commands.push_back(command);
}

void CommandList::ClearDepthTarget(const ClearDepthStencilValue &value)
{
    ClearDepthStencilTargetCommand command;
    command.Value = value;
    m_Commands.push_back(command);
}

void CommandList::SetRenderTarget(RenderTarget target)
{
    m_Commands.push_back(target);
}

void CommandList::SetViewport(const Viewport &viewport)
{
    m_Commands.push_back(viewport);
}

void CommandList::SetScissor(const Scissor &scissor)
{
    m_Commands.push_back(scissor);
}

void CommandList::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
{
    ResolveSamplesToSwapchainCommand command;
    command.Source = source;
    command.SourceIndex = sourceIndex;
    command.Target = target;
    m_Commands.push_back(command);
}

void Nexus::Graphics::CommandList::StartTimingQuery(Ref<TimingQuery> query)
{
    StartTimingQueryCommand command;
    command.Query = query;
    m_Commands.push_back(command);
}

void Nexus::Graphics::CommandList::StopTimingQuery(Ref<TimingQuery> query)
{
    StopTimingQueryCommand command;
    command.Query = query;
    m_Commands.push_back(command);
}

void CommandList::TransitionImageLayout(WeakRef<Texture> texture, uint32_t baseLevel, uint32_t numLevels, ImageLayout layout)
{
    TransitionImageLayoutCommand command;
    command.TransitionTexture = texture;
    command.BaseLevel = baseLevel;
    command.NumLevels = numLevels;
    command.TextureLayout = layout;
    m_Commands.push_back(command);
}

std::vector<RenderCommandData> CommandList::ProcessCommands()
{
    std::vector<RenderCommandData> processed;

    std::vector<RenderCommandData> nonTransitions;
    std::vector<std::vector<RenderCommandData>> transitions(1);

    size_t renderTargetIndex = 0;

    // retrieve the transitions from the queued commands
    // and queue them for the correct render target
    for (size_t i = 0; i < m_Commands.size(); i++)
    {
        RenderCommandData data = m_Commands[i];
        if (std::holds_alternative<TransitionImageLayoutCommand>(data))
        {
            // if the transition was supplied before the first render target was specified,
            // then we do not need to do anything and the transition is able to be added directly to the queue
            if (renderTargetIndex == 0)
            {
                transitions[0].push_back(data);
            }
            // transitions cannot be performed during a render pass, so we need to move any transitions that occur
            // during a render pass before the render pass to prevent errors
            else
            {
                transitions[renderTargetIndex - 1].push_back(data);
            }
        }
        else if (std::holds_alternative<RenderTarget>(data))
        {
            renderTargetIndex++;
            nonTransitions.push_back(data);
            transitions.push_back({});
        }
        else
        {
            nonTransitions.push_back(data);
        }
    }

    renderTargetIndex = 0;
    // insert the queued transitions into the correct places in the command queue
    for (size_t i = 0; i < nonTransitions.size(); i++)
    {
        RenderCommandData data = m_Commands[i];

        // if the next command is a set render target command, insert the queued transitions beforehand
        if (std::holds_alternative<RenderTarget>(data))
        {
            processed.insert(processed.end(), transitions[renderTargetIndex].begin(), transitions[renderTargetIndex].end());
            renderTargetIndex++;
            processed.push_back(data);
        }
        // otherwise, continue inserting non-transition commands in the order that they appear
        else
        {
            processed.push_back(data);
        }
    }

    processed.insert(processed.end(), transitions[renderTargetIndex].begin(), transitions[renderTargetIndex].end());

    NX_ASSERT(processed.size() == m_Commands.size(), "There must be the same of output commands as input commands");

    return processed;
}

const std::vector<RenderCommandData> &CommandList::GetCommandData() const
{
    return m_Commands;
}

const CommandListSpecification &CommandList::GetSpecification()
{
    return m_Specification;
}
} // namespace Nexus::Graphics
