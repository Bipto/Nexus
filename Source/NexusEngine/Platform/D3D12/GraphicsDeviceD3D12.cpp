#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "SwapchainD3D12.hpp"
#include "PipelineD3D12.hpp"
#include "BufferD3D12.hpp"
#include "CommandListD3D12.hpp"
#include "TextureD3D12.hpp"
#include "ResourceSetD3D12.hpp"
#include "FramebufferD3D12.hpp"
#include "SamplerD3D12.hpp"
#include "ShaderModuleD3D12.hpp"

namespace Nexus::Graphics
{
    GraphicsDeviceD3D12::GraphicsDeviceD3D12(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
        : GraphicsDevice(createInfo, window, swapchainSpec)
    {
        // this has to be enabled to support newer HLSL versions and DXIL bytecode
        UUID experimentalFeatures[] = {D3D12ExperimentalShaderModels};
        D3D12EnableExperimentalFeatures(0, nullptr, NULL, NULL);

#if defined(_DEBUG)
        // retrieve the debug interface
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12Debug))))
        {
            // enable debugging of d3d12
            m_D3D12Debug->EnableDebugLayer();
        }

        std::atexit(ReportLiveObjects);
#endif

        if (SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DxgiFactory))))
        {
        }

        // create the D3D12Device
        if (SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device))))
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
    }

    GraphicsDeviceD3D12::~GraphicsDeviceD3D12()
    {
        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
        }
    }

    void GraphicsDeviceD3D12::SetContext()
    {
    }

    void GraphicsDeviceD3D12::SetFramebuffer(Framebuffer *framebuffer)
    {
    }

    void GraphicsDeviceD3D12::SubmitCommandList(Ref<CommandList> commandList)
    {
        Ref<CommandListD3D12> d3d12CommandList = std::dynamic_pointer_cast<CommandListD3D12>(commandList);
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

    Ref<ShaderModule> GraphicsDeviceD3D12::CreateShaderModule(const ShaderModuleSpecification &moduleSpec, const ResourceSetSpecification &resources)
    {
        return CreateRef<ShaderModuleD3D12>(moduleSpec, resources);
    }

    Ref<Texture> GraphicsDeviceD3D12::CreateTexture(const TextureSpecification &spec)
    {
        return CreateRef<TextureD3D12>(this, spec);
    }

    Ref<Pipeline> GraphicsDeviceD3D12::CreatePipeline(const PipelineDescription &description)
    {
        return CreateRef<PipelineD3D12>(m_Device.Get(), description);
    }

    Ref<CommandList> GraphicsDeviceD3D12::CreateCommandList()
    {
        return CreateRef<CommandListD3D12>(this);
    }

    Ref<VertexBuffer> GraphicsDeviceD3D12::CreateVertexBuffer(const BufferDescription &description, const void *data)
    {
        return CreateRef<VertexBufferD3D12>(this, description, data);
    }

    Ref<IndexBuffer> GraphicsDeviceD3D12::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
    {
        return CreateRef<IndexBufferD3D12>(this, description, data, format);
    }

    Ref<UniformBuffer> GraphicsDeviceD3D12::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
        return CreateRef<UniformBufferD3D12>(this, description, data);
    }

    Ref<ResourceSet> GraphicsDeviceD3D12::CreateResourceSet(const ResourceSetSpecification &spec)
    {
        return CreateRef<ResourceSetD3D12>(spec, this);
    }

    Ref<Framebuffer> GraphicsDeviceD3D12::CreateFramebuffer(const FramebufferSpecification &spec)
    {
        return CreateRef<FramebufferD3D12>(spec, this);
    }

    Ref<Sampler> GraphicsDeviceD3D12::CreateSampler(const SamplerSpecification &spec)
    {
        return CreateRef<SamplerD3D12>(spec);
    }

    IDXGIFactory7 *GraphicsDeviceD3D12::GetDXGIFactory() const
    {
        return m_DxgiFactory.Get();
    }

    ID3D12CommandQueue *GraphicsDeviceD3D12::GetCommandQueue() const
    {
        return m_CommandQueue.Get();
    }

    const GraphicsCapabilities GraphicsDeviceD3D12::GetGraphicsCapabilities() const
    {
        GraphicsCapabilities capabilities;
        capabilities.SupportsMultisampledTextures = true;
        capabilities.SupportsLODBias = true;
        return capabilities;
    }

    ID3D12Device10 *GraphicsDeviceD3D12::GetDevice() const
    {
        return m_Device.Get();
    }

    ID3D12GraphicsCommandList7 *GraphicsDeviceD3D12::GetUploadCommandList()
    {
        return m_UploadCommandList.Get();
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

    inline void GraphicsDeviceD3D12::ReportLiveObjects()
    {
        // initialise dxgi debug layer
        Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
        {
            OutputDebugStringW(L"Reporting live D3D12 objects:\n");
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
    }
}

#endif