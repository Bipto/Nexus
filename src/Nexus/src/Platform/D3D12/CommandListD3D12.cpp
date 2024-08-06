#include "CommandListD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "SwapchainD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "BufferD3D12.hpp"
#include "ResourceSetD3D12.hpp"
#include "FramebufferD3D12.hpp"
#include "TimingQueryD3D12.hpp"
#include "TextureD3D12.hpp"
#include "D3D12Utils.hpp"

namespace Nexus::Graphics
{
    CommandListD3D12::CommandListD3D12(GraphicsDeviceD3D12 *device)
    {
        auto d3d12Device = device->GetDevice();
        d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
        d3d12Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_CommandList));
    }

    CommandListD3D12::~CommandListD3D12()
    {
    }

    void CommandListD3D12::Begin()
    {
        m_CommandRecorder.Clear();
    }

    void CommandListD3D12::End()
    {
    }

    void CommandListD3D12::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot)
    {
        SetVertexBufferCommand command;
        command.VertexBufferRef = vertexBuffer;
        command.Slot = slot;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        m_CommandRecorder.PushCommand(indexBuffer);
    }

    void CommandListD3D12::SetPipeline(Ref<Pipeline> pipeline)
    {
        m_CommandRecorder.PushCommand(pipeline);
    }

    void CommandListD3D12::Draw(uint32_t start, uint32_t count)
    {
        DrawElementCommand command;
        command.Start = start;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart)
    {
        DrawIndexedCommand command;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.Count = count;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart)
    {
        DrawInstancedCommand command;
        command.VertexCount = vertexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart)
    {
        DrawInstancedIndexedCommand command;
        command.IndexCount = indexCount;
        command.InstanceCount = instanceCount;
        command.VertexStart = vertexStart;
        command.IndexStart = indexStart;
        command.InstanceStart = instanceStart;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::SetResourceSet(Ref<ResourceSet> resources)
    {
        UpdateResourcesCommand command;
        command.Resources = resources;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::ClearColorTarget(uint32_t index, const ClearColorValue &color)
    {
        ClearColorTargetCommand command;
        command.Index = index;
        command.Color = color;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::ClearDepthTarget(const ClearDepthStencilValue &value)
    {
        ClearDepthStencilTargetCommand command;
        command.Value = value;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::SetRenderTarget(RenderTarget target)
    {
        SetRenderTargetCommand command;
        command.Target = target;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::SetViewport(const Viewport &viewport)
    {
        SetViewportCommand command;
        command.Viewport = viewport;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::SetScissor(const Scissor &scissor)
    {
        SetScissorCommand command;
        command.Scissor = scissor;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target)
    {
        ResolveSamplesToSwapchainCommand command;
        command.Source = source;
        command.SourceIndex = sourceIndex;
        command.Target = target;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::StartTimingQuery(Ref<TimingQuery> query)
    {
        StartTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    void CommandListD3D12::StopTimingQuery(Ref<TimingQuery> query)
    {
        StopTimingQueryCommand command;
        command.Query = query;
        m_CommandRecorder.PushCommand(command);
    }

    ID3D12GraphicsCommandList7 *CommandListD3D12::GetCommandList()
    {
        return m_CommandList.Get();
    }

    void CommandListD3D12::Reset()
    {
        m_CommandAllocator->Reset();
        m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
    }

    void CommandListD3D12::Close()
    {
        m_CommandList->Close();
    }

    CommandRecorder &CommandListD3D12::GetCommandRecorder()
    {
        return m_CommandRecorder;
    }
}

#endif