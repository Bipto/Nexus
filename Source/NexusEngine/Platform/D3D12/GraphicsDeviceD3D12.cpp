#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "SwapchainD3D12.hpp"
#include "ShaderD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "BufferD3D12.hpp"
#include "CommandListD3D12.hpp"
#include "RenderPassD3D12.hpp"
#include "TextureD3D12.hpp"
#include "ResourceSetD3D12.hpp"
#include "FramebufferD3D12.hpp"

namespace Nexus::Graphics
{
    GraphicsDeviceD3D12::GraphicsDeviceD3D12(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
        : GraphicsDevice(createInfo, window, swapchainSpec)
    {
#if defined(_DEBUG)
        // retrieve the debug interface
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12Debug))))
        {
            // enable debugging of d3d12
            m_D3D12Debug->EnableDebugLayer();

            // initialise dxgi debug layer
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_DXGIDebug))))
            {
                // enable leak tracking
                m_DXGIDebug->EnableLeakTrackingForThread();
            }
        }
#endif

        if (SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DxgiFactory))))
        {
        }

        // create the D3D12Device
        if (SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device))))
        {
            // Create a command queue to submit work to the GPU
            D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
            commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
            commandQueueDesc.NodeMask = 0;
            commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

            // create the command queue
            if (SUCCEEDED(m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue))))
            {
                // create the fence
                if (SUCCEEDED(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence))))
                {
                    m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
                }

                m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_UploadCommandAllocator));
                m_Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_UploadCommandList));
            }
        }

        window->CreateSwapchain(this, swapchainSpec);
        CreateImGuiCommandStructures();
    }

    GraphicsDeviceD3D12::~GraphicsDeviceD3D12()
    {
        // release references to objects
        /* m_CommandList->Release();
        m_CommandAllocator->Release();
        m_CommandQueue->Release(); */

        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
        }

        /* m_Fence->Release();
        m_Device->Release(); */

#if defined(_DEBUG)
        // m_D3D12Debug->Release();
        if (m_DXGIDebug)
        {
            // report any remaining live objects
            OutputDebugStringW(L"Reporting live D3D12 objects:\n");
            m_DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
            // m_DXGIDebug->Release();
        }

        // m_DxgiFactory->Release();
#endif
    }

    void GraphicsDeviceD3D12::SetContext()
    {
    }

    void GraphicsDeviceD3D12::SetFramebuffer(Framebuffer *framebuffer)
    {
    }

    void GraphicsDeviceD3D12::SubmitCommandList(CommandList *commandList)
    {
        CommandListD3D12 *d3d12CommandList = (CommandListD3D12 *)commandList;
        ID3D12CommandList *lists[] = {d3d12CommandList->GetCommandList()};
        m_CommandQueue->ExecuteCommandLists(1, lists);
        SignalAndWait();
    }

    const std::string GraphicsDeviceD3D12::GetAPIName()
    {
        return {"D3D12"};
    }

    const char *GraphicsDeviceD3D12::GetDeviceName()
    {
        return nullptr;
    }

    void *GraphicsDeviceD3D12::GetContext()
    {
        return nullptr;
    }

    void GraphicsDeviceD3D12::BeginFrame()
    {
    }

    void GraphicsDeviceD3D12::EndFrame()
    {
    }

    Shader *GraphicsDeviceD3D12::CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
        return new ShaderD3D12(vertexShaderSource, fragmentShaderSource, layout);
    }

    Texture *GraphicsDeviceD3D12::CreateTexture(const TextureSpecification &spec)
    {
        return new TextureD3D12(this, spec);
    }

    Framebuffer *GraphicsDeviceD3D12::CreateFramebuffer(RenderPass *renderPass)
    {
        RenderPassD3D12 *d3d12RenderPass = (RenderPassD3D12 *)renderPass;
        auto framebuffer = new FramebufferD3D12(this, renderPass);
        d3d12RenderPass->m_Framebuffer = framebuffer;
        return framebuffer;
    }

    Pipeline *GraphicsDeviceD3D12::CreatePipeline(const PipelineDescription &description)
    {
        return new PipelineD3D12(m_Device.Get(), description);
    }

    CommandList *GraphicsDeviceD3D12::CreateCommandList()
    {
        return new CommandListD3D12(this);
    }

    VertexBuffer *GraphicsDeviceD3D12::CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
    {
        return new VertexBufferD3D12(this, description, data, layout);
    }

    IndexBuffer *GraphicsDeviceD3D12::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
    {
        return new IndexBufferD3D12(this, description, data, format);
    }

    UniformBuffer *GraphicsDeviceD3D12::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
        return new UniformBufferD3D12(this, description, data);
    }

    RenderPass *GraphicsDeviceD3D12::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification)
    {
        return new RenderPassD3D12(renderPassSpecification, framebufferSpecification);
    }

    RenderPass *GraphicsDeviceD3D12::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        return new RenderPassD3D12(renderPassSpecification, swapchain);
    }

    ResourceSet *GraphicsDeviceD3D12::CreateResourceSet(const ResourceSetSpecification &spec)
    {
        return new ResourceSetD3D12(spec, this);
    }

    Framebuffer *GraphicsDeviceD3D12::CreateFramebuffer(const FramebufferSpecification &spec)
    {
        return nullptr;
    }

    IDXGIFactory7 *GraphicsDeviceD3D12::GetDXGIFactory() const
    {
        return m_DxgiFactory.Get();
    }

    ID3D12CommandQueue *GraphicsDeviceD3D12::GetCommandQueue() const
    {
        return m_CommandQueue.Get();
    }

    ID3D12Device10 *GraphicsDeviceD3D12::GetDevice() const
    {
        return m_Device.Get();
    }

    ID3D12GraphicsCommandList7 *GraphicsDeviceD3D12::GetUploadCommandList()
    {
        return m_UploadCommandList.Get();
    }

    ID3D12GraphicsCommandList7 *GraphicsDeviceD3D12::GetImGuiCommandList()
    {
        return m_ImGuiCommandList.Get();
    }

    ID3D12DescriptorHeap *GraphicsDeviceD3D12::GetImGuiDescriptorHeap()
    {
        return m_ImGuiDescriptorHeap.Get();
    }

    void GraphicsDeviceD3D12::SignalAndWait()
    {
        m_CommandQueue->Signal(m_Fence.Get(), ++m_FenceValue);

        if (SUCCEEDED(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent)))
        {
            if (WaitForSingleObject(m_FenceEvent, 20000) != WAIT_OBJECT_0)
            {
                throw std::runtime_error("Failed to wait for fence event");
            }
        }
        else
        {
            throw std::runtime_error("Failed to set fence event");
        }
    }

    void GraphicsDeviceD3D12::ImmediateSubmit(std::function<void(ID3D12GraphicsCommandList7 *cmd)> &&function)
    {
        InitUploadCommandList();
        function(m_UploadCommandList.Get());
        DispatchUploadCommandList();
        SignalAndWait();
    }

    void GraphicsDeviceD3D12::BeginImGuiFrame()
    {
        m_ImGuiCommandAllocator->Reset();
        m_ImGuiCommandList->Reset(m_ImGuiCommandAllocator.Get(), nullptr);

        D3D12_RESOURCE_BARRIER resourceBarrier;
        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

        SwapchainD3D12 *d3d12Swapchain = (SwapchainD3D12 *)m_Window->GetSwapchain();
        resourceBarrier.Transition.pResource = d3d12Swapchain->RetrieveBufferHandles()[d3d12Swapchain->GetCurrentBufferIndex()];
        resourceBarrier.Transition.Subresource = 0;
        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_ImGuiCommandList->ResourceBarrier(1, &resourceBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE targets[] =
            {
                d3d12Swapchain->RetrieveRenderTargetViewDescriptorHandles()[d3d12Swapchain->GetCurrentBufferIndex()]};

        m_ImGuiCommandList->OMSetRenderTargets(
            1,
            targets,
            false,
            nullptr);

        const ID3D12DescriptorHeap *heaps[] = {m_ImGuiDescriptorHeap.Get()};
        // m_ImGuiCommandList->SetGraphicsRootSignature(m_ImGuiRootSignature.Get());
        m_ImGuiCommandList->SetDescriptorHeaps(1, (ID3D12DescriptorHeap *const *)heaps);
        // m_ImGuiCommandList->SetGraphicsRootDescriptorTable(0, m_ImGuiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }

    void GraphicsDeviceD3D12::EndImGuiFrame()
    {
        SwapchainD3D12 *d3d12Swapchain = (SwapchainD3D12 *)m_Window->GetSwapchain();

        D3D12_RESOURCE_BARRIER resourceBarrier;
        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        resourceBarrier.Transition.pResource = d3d12Swapchain->RetrieveBufferHandles()[d3d12Swapchain->GetCurrentBufferIndex()];
        resourceBarrier.Transition.Subresource = 0;
        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        m_ImGuiCommandList->ResourceBarrier(1, &resourceBarrier);

        if (SUCCEEDED(m_ImGuiCommandList->Close()))
        {
            ID3D12CommandList *list[] = {m_ImGuiCommandList.Get()};
            m_CommandQueue->ExecuteCommandLists(1, list);
            SignalAndWait();
        }
    }

    uint32_t GraphicsDeviceD3D12::GetNextTextureOffset()
    {
        return m_DescriptorHandleOffset++;
    }

    void GraphicsDeviceD3D12::InitUploadCommandList()
    {
        m_UploadCommandAllocator->Reset();
        m_UploadCommandList->Reset(m_UploadCommandAllocator.Get(), nullptr);
    }

    void GraphicsDeviceD3D12::DispatchUploadCommandList()
    {
        if (SUCCEEDED(m_UploadCommandList->Close()))
        {
            ID3D12CommandList *list[] = {m_UploadCommandList.Get()};
            m_CommandQueue->ExecuteCommandLists(1, list);
            SignalAndWait();
        }
    }

    void GraphicsDeviceD3D12::CreateImGuiCommandStructures()
    {
        m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_ImGuiCommandAllocator));
        m_Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_ImGuiCommandList));

        D3D12_DESCRIPTOR_RANGE texRange;
        texRange.BaseShaderRegister = 0;
        texRange.NumDescriptors = 1000;
        texRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        texRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        texRange.RegisterSpace = 0;

        D3D12_ROOT_DESCRIPTOR_TABLE texTable;
        texTable.NumDescriptorRanges = 1;
        texTable.pDescriptorRanges = &texRange;

        D3D12_ROOT_PARAMETER texParameter;
        texParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        texParameter.DescriptorTable = texTable;
        texParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rootSignatureDesc.NumParameters = 0;
        rootSignatureDesc.NumStaticSamplers = 0;
        rootSignatureDesc.NumParameters = 1;
        rootSignatureDesc.pParameters = &texParameter;

        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        if (SUCCEEDED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &m_ImGuiRootSignatureBlob, &errorBlob)))
        {
            m_Device->CreateRootSignature(0, m_ImGuiRootSignatureBlob->GetBufferPointer(), m_ImGuiRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_ImGuiRootSignature));
        }

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.NumDescriptors = 1000;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDesc.NodeMask = 0;

        ID3D12DescriptorHeap *descriptorHeap = nullptr;
        m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_ImGuiDescriptorHeap));
    }
}

#endif