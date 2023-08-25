#include "GraphicsDeviceDX11.hpp"
#include "Core/Logging/Log.hpp"

#include "BufferDX11.hpp"
#include "ShaderDX11.hpp"
#include "TextureDX11.hpp"
#include "PipelineDX11.hpp"
#include "CommandListDX11.hpp"
#include "RenderPassDX11.hpp"

#include "SDL_syswm.h"

namespace Nexus::Graphics
{
    GraphicsDeviceDX11::GraphicsDeviceDX11(const GraphicsDeviceCreateInfo &createInfo, Window *window)
        : GraphicsDevice(createInfo, window)
    {
#if defined(NX_PLATFORM_DX11)
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(m_Window->GetSDLWindowHandle(), &wmInfo);
        HWND hwnd = wmInfo.info.win.window;

        auto windowSize = m_Window->GetWindowSize();

        DXGI_SWAP_CHAIN_DESC swap_chain_desc;
        ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
        swap_chain_desc.BufferDesc.Width = windowSize.X;
        swap_chain_desc.BufferDesc.Height = windowSize.Y;
        swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
        swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
        swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = 3;
        swap_chain_desc.OutputWindow = hwnd;
        swap_chain_desc.Windowed = true;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

        D3D_FEATURE_LEVEL feature_level;
        UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(DEBUG) | defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        IDXGISwapChain *swapchain;
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            flags,
            NULL,
            0,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &swapchain,
            &m_DevicePtr,
            &feature_level,
            &m_DeviceContextPtr);

        m_Swapchain = new SwapchainDX11(m_Window, m_DevicePtr, swapchain, createInfo.VSyncStateSettings);
        m_ActiveRenderTargetviews = {m_Swapchain->GetRenderTargetView()};
        m_ActiveDepthStencilView = m_Swapchain->GetDepthStencilView();

        m_DeviceContextPtr->OMSetRenderTargets(
            m_ActiveRenderTargetviews.size(),
            m_ActiveRenderTargetviews.data(),
            m_ActiveDepthStencilView);

        IDXGIFactory1 *factory;
        IDXGIAdapter1 *adapter;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));
        factory->EnumAdapters1(0, &adapter);
        DXGI_ADAPTER_DESC1 adapterDesc;
        adapter->GetDesc1(&adapterDesc);
        std::wstring ws(adapterDesc.Description);
        m_AdapterName = std::string(ws.begin(), ws.end());
#endif
    }

    void GraphicsDeviceDX11::SetContext()
    {
    }

    void GraphicsDeviceDX11::SetFramebuffer(Ref<Framebuffer> framebuffer)
    {
#if defined(NX_PLATFORM_DX11)
        if (framebuffer)
        {
            Ref<FramebufferDX11> dxFramebuffer = std::dynamic_pointer_cast<FramebufferDX11>(framebuffer);

            std::vector<ID3D11RenderTargetView *> colorTargets;
            for (const auto &colorTarget : dxFramebuffer->GetColorRenderTargets())
            {
                colorTargets.push_back(colorTarget.RenderTargetView);
            }

            m_ActiveRenderTargetviews = colorTargets;
            m_ActiveDepthStencilView = dxFramebuffer->GetDepthStencilView();
        }
        else
        {
            m_ActiveRenderTargetviews = {m_Swapchain->GetRenderTargetView()};
            m_ActiveDepthStencilView = m_Swapchain->GetDepthStencilView();
        }

        m_DeviceContextPtr->OMSetRenderTargets(
            m_ActiveRenderTargetviews.size(),
            m_ActiveRenderTargetviews.data(),
            m_ActiveDepthStencilView);
#endif
    }

    void GraphicsDeviceDX11::SubmitCommandList(Ref<CommandList> commandList)
    {
        Ref<CommandListDX11> commandListDX11 = std::dynamic_pointer_cast<CommandListDX11>(commandList);
        auto &commands = commandListDX11->GetRenderCommands();
        for (auto &command : commands)
        {
            command(commandList);
        }
    }

    void GraphicsDeviceDX11::SetViewport(const Viewport &viewport)
    {
        m_Viewport = viewport;

#if defined(NX_PLATFORM_DX11)
        D3D11_VIEWPORT vp;
        vp.Width = (float)viewport.Width;
        vp.Height = (float)viewport.Height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = viewport.X;
        vp.TopLeftY = viewport.Y;
        m_DeviceContextPtr->RSSetViewports(1, &vp);
#endif
    }

    const std::string GraphicsDeviceDX11::GetAPIName()
    {
        return {"DirectX11"};
    }

    const char *GraphicsDeviceDX11::GetDeviceName()
    {
#if defined(NX_PLATFORM_DX11)
        return m_AdapterName.c_str();
#else
        return "Device";
#endif
    }

    void *GraphicsDeviceDX11::GetContext()
    {
#if defined(NX_PLATFORM_DX11)
        return m_DeviceContextPtr;
#else
        return nullptr;
#endif
    }

    void GraphicsDeviceDX11::BeginFrame()
    {
    }

    void GraphicsDeviceDX11::EndFrame()
    {
    }

    Ref<Shader> GraphicsDeviceDX11::CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
#if defined(NX_PLATFORM_DX11)
        return CreateRef<ShaderDX11>(m_DevicePtr, m_DeviceContextPtr, vertexShaderSource, fragmentShaderSource, layout);
#else
        return nullptr;
#endif
    }

    Ref<Texture> GraphicsDeviceDX11::CreateTexture(const TextureSpecification &spec)
    {
#if defined(NX_PLATFORM_DX11)
        return CreateRef<TextureDX11>(m_DevicePtr, spec);
#else
        return nullptr;
#endif
    }

    Ref<Framebuffer> GraphicsDeviceDX11::CreateFramebuffer(Ref<RenderPass> renderPass)
    {
#if defined(NX_PLATFORM_DX11)
        auto framebufferDX11 = CreateRef<FramebufferDX11>(m_DevicePtr, renderPass);
        auto renderPassDX11 = std::dynamic_pointer_cast<RenderPassDX11>(renderPass);
        renderPassDX11->m_Framebuffer = framebufferDX11;
        return framebufferDX11;
#else
        return nullptr;
#endif
    }

    Ref<Pipeline> GraphicsDeviceDX11::CreatePipeline(const PipelineDescription &description)
    {
#if defined(NX_PLATFORM_DX11)
        return CreateRef<PipelineDX11>(m_DevicePtr, description);
#else
        return nullptr;
#endif
    }

    Ref<CommandList> GraphicsDeviceDX11::CreateCommandList()
    {
#if defined(NX_PLATFORM_DX11)
        return CreateRef<CommandListDX11>(this);
#else
        return nullptr;
#endif
    }

    Ref<VertexBuffer> GraphicsDeviceDX11::CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
    {
#if defined(NX_PLATFORM_DX11)
        return CreateRef<VertexBufferDX11>(m_DevicePtr, m_DeviceContextPtr, description, data, layout);
#else
        return nullptr;
#endif
    }

    Ref<IndexBuffer> GraphicsDeviceDX11::CreateIndexBuffer(const BufferDescription &description, const void *data)
    {
#if defined(NX_PLATFORM_DX11)
        return CreateRef<IndexBufferDX11>(m_DevicePtr, m_DeviceContextPtr, description, data);
#else
        return nullptr;
#endif
    }

    Ref<UniformBuffer> GraphicsDeviceDX11::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
#if defined(NX_PLATFORM_DX11)
        return CreateRef<UniformBufferDX11>(m_DevicePtr, m_DeviceContextPtr, description, data);
#else
        return nullptr;
#endif
    }

    Ref<RenderPass> GraphicsDeviceDX11::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification)
    {
        return CreateRef<RenderPassDX11>(renderPassSpecification, framebufferSpecification);
    }

    Ref<RenderPass> GraphicsDeviceDX11::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        return CreateRef<RenderPassDX11>(renderPassSpecification, swapchain);
    }

    void GraphicsDeviceDX11::Resize(Point<int> size)
    {
#if defined(NX_PLATFORM_DX11)

        m_DeviceContextPtr->OMSetRenderTargets(0, 0, 0);

        m_Swapchain->Resize(size.X, size.Y);

        m_ActiveRenderTargetviews = {m_Swapchain->GetRenderTargetView()};
        m_ActiveDepthStencilView = m_Swapchain->GetDepthStencilView();

        m_DeviceContextPtr->OMSetRenderTargets(
            m_ActiveRenderTargetviews.size(),
            m_ActiveRenderTargetviews.data(),
            m_ActiveDepthStencilView);

#endif
    }

    Swapchain *GraphicsDeviceDX11::GetSwapchain()
    {
        return m_Swapchain;
    }

#if defined(NX_PLATFORM_DX11)
    ID3D11DeviceContext *GraphicsDeviceDX11::GetDeviceContext()
    {
        return m_DeviceContextPtr;
    }

    std::vector<ID3D11RenderTargetView *> &GraphicsDeviceDX11::GetActiveRenderTargetViews()
    {
        return m_ActiveRenderTargetviews;
    }

    ID3D11DepthStencilView *&GraphicsDeviceDX11::GetActiveDepthStencilView()
    {
        return m_ActiveDepthStencilView;
    }
#endif
}