#include "GraphicsDeviceD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

namespace Nexus::Graphics
{
    GraphicsDeviceD3D12::GraphicsDeviceD3D12(const GraphicsDeviceCreateInfo &createInfo, Window *window)
        : GraphicsDevice(createInfo, window)
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

                m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
                m_Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_CommandList));
            }
        }
    }

    GraphicsDeviceD3D12::~GraphicsDeviceD3D12()
    {

        // release references to objects
        m_CommandList->Release();
        m_CommandAllocator->Release();

        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
        }

        m_Fence->Release();
        m_Device->Release();

#if defined(_DEBUG)
        if (m_DXGIDebug)
        {
            // report any remaining live objects
            OutputDebugStringW(L"Reporting live D3D12 objects:");
            m_DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }

        // release remaining references
        m_DXGIDebug->Release();
        m_D3D12Debug->Release();
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
    }

    const std::string GraphicsDeviceD3D12::GetAPIName()
    {
        return std::string();
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
        return nullptr;
    }

    Texture *GraphicsDeviceD3D12::CreateTexture(const TextureSpecification &spec)
    {
        return nullptr;
    }

    Framebuffer *GraphicsDeviceD3D12::CreateFramebuffer(RenderPass *renderPass)
    {
        return nullptr;
    }

    Pipeline *GraphicsDeviceD3D12::CreatePipeline(const PipelineDescription &description)
    {
        return nullptr;
    }

    CommandList *GraphicsDeviceD3D12::CreateCommandList()
    {
        return nullptr;
    }

    VertexBuffer *GraphicsDeviceD3D12::CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
    {
        return nullptr;
    }

    IndexBuffer *GraphicsDeviceD3D12::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
    {
        return nullptr;
    }

    UniformBuffer *GraphicsDeviceD3D12::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
        return nullptr;
    }

    RenderPass *GraphicsDeviceD3D12::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification)
    {
        return nullptr;
    }

    RenderPass *GraphicsDeviceD3D12::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        return nullptr;
    }

    ResourceSet *GraphicsDeviceD3D12::CreateResourceSet(const ResourceSetSpecification &spec)
    {
        return nullptr;
    }

    void GraphicsDeviceD3D12::SignalAndWait()
    {
        m_CommandQueue->Signal(m_Fence, ++m_FenceValue);

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

    void GraphicsDeviceD3D12::InitCommandList()
    {
        m_CommandAllocator->Reset();
        m_CommandList->Reset(m_CommandAllocator, nullptr);
    }

    void GraphicsDeviceD3D12::DispatchCommandList()
    {
        if (SUCCEEDED(m_CommandList->Close()))
        {
            ID3D12CommandList *list[] = {m_CommandList};
            m_CommandQueue->ExecuteCommandLists(1, list);
            SignalAndWait();
        }
    }
}

#endif