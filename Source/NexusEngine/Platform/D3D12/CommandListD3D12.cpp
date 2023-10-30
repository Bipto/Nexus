#include "CommandListD3D12.hpp"

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
    }

    void CommandListD3D12::End()
    {
    }

    void CommandListD3D12::BeginRenderPass(RenderPass *renderPass, const RenderPassBeginInfo &beginInfo)
    {
    }

    void CommandListD3D12::EndRenderPass()
    {
    }

    void CommandListD3D12::SetVertexBuffer(VertexBuffer *vertexBuffer)
    {
    }

    void CommandListD3D12::SetIndexBuffer(IndexBuffer *indexBuffer)
    {
    }

    void CommandListD3D12::SetPipeline(Pipeline *pipeline)
    {
    }

    void CommandListD3D12::DrawElements(uint32_t start, uint32_t count)
    {
    }

    void CommandListD3D12::DrawIndexed(uint32_t count, uint32_t offset)
    {
    }

    void CommandListD3D12::SetResourceSet(ResourceSet *resources)
    {
    }
}