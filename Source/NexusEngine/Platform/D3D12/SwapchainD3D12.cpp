#include "SwapchainD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "SDL_syswm.h"

namespace Nexus::Graphics
{
    SwapchainD3D12::SwapchainD3D12(Window *window, GraphicsDevice *device, const SwapchainSpecification &swapchainSpec)
        : Swapchain(swapchainSpec), m_Window(window), m_VsyncState(swapchainSpec.VSyncState)
    {
        // assign the graphics device
        m_Device = (GraphicsDeviceD3D12 *)device;

        // setup colour attachments for swapchain
        CreateColourAttachments();
        CreateDepthAttachment();
        CreateMultisampledFramebuffer();
    }

    Nexus::Graphics::SwapchainD3D12::~SwapchainD3D12()
    {
        // we need to flush the swapchain to ensure that resources are not in use when we attempt to delete them
        Flush();

        // release the swapchain's buffers
        ReleaseBuffers();
    }

    void SwapchainD3D12::SwapBuffers()
    {
        // swap the swapchain's buffers and present to the display
        m_Swapchain->Present((uint32_t)m_VsyncState, 0);

        std::cout << "Width: " << m_SwapchainWidth << "\n";
        std::cout << "Height: " << m_SwapchainHeight << "\n";
    }

    VSyncState SwapchainD3D12::GetVsyncState()
    {
        return m_VsyncState;
    }

    void SwapchainD3D12::SetVSyncState(VSyncState vsyncState)
    {
        m_VsyncState = vsyncState;
    }

    void SwapchainD3D12::Prepare()
    {
        // recreate the swapchain if the window's size has changed
        RecreateSwapchainIfNecessary();

        // retrieve the current buffer index from the swapchain
        m_CurrentBufferIndex = m_Swapchain->GetCurrentBackBufferIndex();
    }

    ID3D12Resource2 *SwapchainD3D12::RetrieveBufferHandle()
    {
        return m_Buffers.at(m_CurrentBufferIndex);
    }

    uint32_t SwapchainD3D12::GetCurrentBufferIndex()
    {
        return m_CurrentBufferIndex;
    }

    const D3D12_CPU_DESCRIPTOR_HANDLE SwapchainD3D12::RetrieveRenderTargetViewDescriptorHandle() const
    {
        return m_RenderTargetViewDescriptorHandles.at(m_CurrentBufferIndex);
    }

    ID3D12Resource2 *SwapchainD3D12::RetrieveDepthBufferHandle()
    {
        return m_DepthBuffer.Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE SwapchainD3D12::RetrieveDepthBufferDescriptorHandle()
    {
        return m_DepthTextureDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    }

    uint32_t SwapchainD3D12::GetColorAttachmentCount()
    {
        return 1;
    }

    const D3D12_RESOURCE_STATES SwapchainD3D12::GetCurrentTextureState() const
    {
        return m_CurrentTextureStates.at(m_CurrentBufferIndex);
    }

    const D3D12_RESOURCE_STATES SwapchainD3D12::GetCurrentDepthState() const
    {
        return m_CurrentDepthState;
    }

    void SwapchainD3D12::SetTextureState(D3D12_RESOURCE_STATES state)
    {
        m_CurrentTextureStates[m_CurrentBufferIndex] = state;
    }

    void SwapchainD3D12::SetDepthState(D3D12_RESOURCE_STATES state)
    {
        m_CurrentDepthState = state;
    }

    Framebuffer *SwapchainD3D12::GetMultisampledFramebuffer()
    {
        return m_MultisampledFramebuffer;
    }

    void SwapchainD3D12::Flush()
    {
        // execute a signal and wait for each buffer in the swapchain
        for (int i = 0; i < BUFFER_COUNT; i++)
        {
            m_Device->SignalAndWait();
        }
    }

    void SwapchainD3D12::RecreateSwapchainIfNecessary()
    {
        auto windowWidth = m_Window->GetWindowSize().X;
        auto windowHeight = m_Window->GetWindowSize().Y;

        // if the size of the window is the same, we do not need to do anything and can return
        if (m_SwapchainWidth == windowWidth || m_SwapchainHeight == windowHeight)
            return;

        m_SwapchainWidth = windowWidth;
        m_SwapchainHeight = windowHeight;

        // resize the swapchain
        ResizeBuffers(m_SwapchainWidth, m_SwapchainHeight);
        CreateDepthAttachment();
        CreateMultisampledFramebuffer();
    }

    void SwapchainD3D12::ResizeBuffers(uint32_t width, uint32_t height)
    {
        // flush swapchain to ensure that buffers are not in use
        Flush();

        // we need to release the buffers retrieved from the swapchain before new ones can be created
        ReleaseBuffers();

        // resize the swapchains buffers
        m_Swapchain->ResizeBuffers(BUFFER_COUNT, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

        // retrieve the new buffers
        GetBuffers();
    }

    void SwapchainD3D12::GetBuffers()
    {
        const auto d3d12Device = m_Device->GetDevice();

        // loop through and retrieve the buffers from the swapchain
        for (size_t i = 0; i < BUFFER_COUNT; ++i)
        {
            m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_Buffers[i]));

            D3D12_RENDER_TARGET_VIEW_DESC rtv;
            rtv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtv.Texture2D.MipSlice = 0;
            rtv.Texture2D.PlaneSlice = 0;

            d3d12Device->CreateRenderTargetView(m_Buffers[i], &rtv, m_RenderTargetViewDescriptorHandles[i]);
        }
    }

    void SwapchainD3D12::ReleaseBuffers()
    {
        // loop through retrieved buffers and clean them up
        for (size_t i = 0; i < BUFFER_COUNT; ++i)
        {
            m_Buffers[i]->Release();
        }
    }
    void SwapchainD3D12::CreateColourAttachments()
    {
        // resize the buffer vector to a suitable size
        m_Buffers.resize(BUFFER_COUNT);
        m_RenderTargetViewDescriptorHandles.resize(BUFFER_COUNT);
        m_CurrentTextureStates.clear();

        // retrieve the window's native handle
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(m_Window->GetSDLWindowHandle(), &wmInfo);
        HWND hwnd = wmInfo.info.win.window;

        // create the swapchain
        auto windowSize = m_Window->GetWindowSize();

        // set up properties for the swapchain
        DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
        swapchainDesc.Width = windowSize.X;
        swapchainDesc.Height = windowSize.Y;
        swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchainDesc.Stereo = false;
        swapchainDesc.SampleDesc.Count = 1;
        swapchainDesc.SampleDesc.Quality = 0;
        swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.BufferCount = BUFFER_COUNT;
        swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        // create a fullscreen description, we will not use exclusive fullscreen so we don't need this
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc{};
        fullscreenDesc.Windowed = true;

        // retrieve the graphics device's DXGI factory
        auto factory = m_Device->GetDXGIFactory();

        // create the swapchain and query for the correct swapchain type
        IDXGISwapChain1 *sc1;
        factory->CreateSwapChainForHwnd(m_Device->GetCommandQueue(), hwnd, &swapchainDesc, &fullscreenDesc, nullptr, &sc1);
        if (SUCCEEDED(sc1->QueryInterface(IID_PPV_ARGS(&m_Swapchain))))
        {
            // release the original swapchain, otherwise we create a memory leak
            sc1->Release();
        }

        // retrieve the ID3D12Device
        const auto d3d12Device = m_Device->GetDevice();

        // create the descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descriptorHeapDesc.NumDescriptors = BUFFER_COUNT;
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descriptorHeapDesc.NodeMask = 0;

        d3d12Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_RenderTargetViewDescriptorHeap));

        // create handles to descriptor view
        auto firstHandle = m_RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        auto handleIncrement = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (int i = 0; i < BUFFER_COUNT; ++i)
        {
            m_RenderTargetViewDescriptorHandles[i] = firstHandle;
            m_RenderTargetViewDescriptorHandles[i].ptr += handleIncrement * i;
            m_CurrentTextureStates.push_back(D3D12_RESOURCE_STATE_COMMON);
        }

        // get the buffers from the swapchain
        GetBuffers();
    }

    void SwapchainD3D12::CreateDepthAttachment()
    {
        auto d3d12Device = m_Device->GetDevice();
        auto size = m_Window->GetWindowSize();
        m_CurrentDepthState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        d3d12Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DepthTextureDescriptorHeap));

        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        D3D12_HEAP_PROPERTIES heapProperties;
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.CreationNodeMask = 0;
        heapProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC resourceDesc;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        resourceDesc.Width = size.X;
        resourceDesc.Height = size.Y;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        d3d12Device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            m_CurrentDepthState,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&m_DepthBuffer));

        d3d12Device->CreateDepthStencilView(m_DepthBuffer.Get(), &depthStencilDesc, m_DepthTextureDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }

    void SwapchainD3D12::CreateMultisampledFramebuffer()
    {
        Nexus::Graphics::FramebufferSpecification spec;
        spec.Width = m_SwapchainWidth;
        spec.Height = m_SwapchainHeight;
        spec.ColorAttachmentSpecification = {Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm};
        spec.DepthAttachmentSpecification = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;
        spec.Samples = m_Specification.Samples;

        m_MultisampledFramebuffer = m_Device->CreateFramebuffer(spec);
    }
}
#endif