#if defined(NX_PLATFORM_D3D11)

#include "GraphicsDeviceD3D11.hpp"
#include "Nexus/Logging/Log.hpp"

#include "BufferD3D11.hpp"
#include "ShaderD3D11.hpp"
#include "TextureD3D11.hpp"
#include "PipelineD3D11.hpp"
#include "CommandListD3D11.hpp"
#include "RenderPassD3D11.hpp"
#include "ResourceSetD3D11.hpp"

#include "SDL_syswm.h"

namespace Nexus::Graphics
{
    GraphicsDeviceD3D11::GraphicsDeviceD3D11(const GraphicsDeviceCreateInfo &createInfo, Window *window, const SwapchainSpecification &swapchainSpec)
        : GraphicsDevice(createInfo, window, swapchainSpec)
    {
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

        HRESULT hr = D3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            flags,
            NULL,
            0,
            D3D11_SDK_VERSION,
            &m_DevicePtr,
            &feature_level,
            &m_DeviceContextPtr);

        m_DeviceContextPtr->Release();

        m_Window->CreateSwapchain(this, swapchainSpec);

        IDXGIFactory1 *factory;
        IDXGIAdapter1 *adapter;

        CreateDXGIFactory1(IID_PPV_ARGS(&factory));
        factory->EnumAdapters1(0, &adapter);
        DXGI_ADAPTER_DESC1 adapterDesc;
        adapter->GetDesc1(&adapterDesc);
        std::wstring ws(adapterDesc.Description);
        m_AdapterName = std::string(ws.begin(), ws.end());

        factory->Release();
        adapter->Release();

        m_DeviceContextPtr->AddRef();
    }

    GraphicsDeviceD3D11::~GraphicsDeviceD3D11()
    {
        m_DevicePtr->Release();

        m_DeviceContextPtr->ClearState();
        m_DeviceContextPtr->Flush();
        m_DeviceContextPtr->Release();
    }

    void GraphicsDeviceD3D11::SetContext()
    {
    }

    void GraphicsDeviceD3D11::SetFramebuffer(Framebuffer *framebuffer)
    {
        auto dxFramebuffer = (FramebufferD3D11 *)framebuffer;

        std::vector<ID3D11RenderTargetView *> colorTargets;
        for (const auto &colorTarget : dxFramebuffer->GetColorRenderTargets())
        {
            colorTargets.push_back(colorTarget.RenderTargetView);
        }

        m_ActiveRenderTargetviews = colorTargets;
        m_ActiveDepthStencilView = dxFramebuffer->GetDepthStencilView();

        m_DeviceContextPtr->OMSetRenderTargets(
            m_ActiveRenderTargetviews.size(),
            m_ActiveRenderTargetviews.data(),
            m_ActiveDepthStencilView);
    }

    void GraphicsDeviceD3D11::SetSwapchain(Swapchain *swapchain)
    {
        SwapchainD3D11 *d3d11Swapchain = (SwapchainD3D11 *)swapchain;
        m_ActiveRenderTargetviews = {d3d11Swapchain->GetRenderTargetView()};
        m_ActiveDepthStencilView = d3d11Swapchain->GetDepthStencilView();

        m_DeviceContextPtr->OMSetRenderTargets(
            m_ActiveRenderTargetviews.size(),
            m_ActiveRenderTargetviews.data(),
            m_ActiveDepthStencilView);
    }

    void GraphicsDeviceD3D11::SubmitCommandList(CommandList *commandList)
    {
        auto commandListD3D11 = (CommandListD3D11 *)commandList;
        auto &commands = commandListD3D11->GetRenderCommands();
        for (auto &command : commands)
        {
            command(commandList);
        }
    }

    const std::string GraphicsDeviceD3D11::GetAPIName()
    {
        return {"Direct3D11"};
    }

    const char *GraphicsDeviceD3D11::GetDeviceName()
    {
        return m_AdapterName.c_str();
    }

    void *GraphicsDeviceD3D11::GetContext()
    {
        return m_DeviceContextPtr;
    }

    void GraphicsDeviceD3D11::BeginFrame()
    {
    }

    void GraphicsDeviceD3D11::EndFrame()
    {
    }

    Shader *GraphicsDeviceD3D11::CreateShaderFromSource(const std::string &vertexShaderSource, const std::string &fragmentShaderSource, const VertexBufferLayout &layout)
    {
        return new ShaderD3D11(m_DevicePtr, m_DeviceContextPtr, vertexShaderSource, fragmentShaderSource, layout);
    }

    Texture *GraphicsDeviceD3D11::CreateTexture(const TextureSpecification &spec)
    {
        return new TextureD3D11(m_DevicePtr, spec);
    }

    Framebuffer *GraphicsDeviceD3D11::CreateFramebuffer(RenderPass *renderPass)
    {
        auto renderPassD3D11 = (RenderPassD3D11 *)renderPass;
        auto framebufferD3D11 = new FramebufferD3D11(m_DevicePtr, renderPass);
        renderPassD3D11->m_Framebuffer = framebufferD3D11;
        return framebufferD3D11;
    }

    Pipeline *GraphicsDeviceD3D11::CreatePipeline(const PipelineDescription &description)
    {
        return new PipelineD3D11(m_DevicePtr, m_DeviceContextPtr, description);
    }

    CommandList *GraphicsDeviceD3D11::CreateCommandList()
    {
        return new CommandListD3D11(this);
    }

    VertexBuffer *GraphicsDeviceD3D11::CreateVertexBuffer(const BufferDescription &description, const void *data, const VertexBufferLayout &layout)
    {
        return new VertexBufferD3D11(m_DevicePtr, m_DeviceContextPtr, description, data, layout);
    }

    IndexBuffer *GraphicsDeviceD3D11::CreateIndexBuffer(const BufferDescription &description, const void *data, IndexBufferFormat format)
    {
        return new IndexBufferD3D11(m_DevicePtr, m_DeviceContextPtr, description, data, format);
    }

    UniformBuffer *GraphicsDeviceD3D11::CreateUniformBuffer(const BufferDescription &description, const void *data)
    {
        return new UniformBufferD3D11(m_DevicePtr, m_DeviceContextPtr, description, data);
    }

    RenderPass *GraphicsDeviceD3D11::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, const FramebufferSpecification &framebufferSpecification)
    {
        return new RenderPassD3D11(renderPassSpecification, framebufferSpecification);
    }

    RenderPass *GraphicsDeviceD3D11::CreateRenderPass(const RenderPassSpecification &renderPassSpecification, Swapchain *swapchain)
    {
        return new RenderPassD3D11(renderPassSpecification, swapchain);
    }

    ResourceSet *GraphicsDeviceD3D11::CreateResourceSet(const ResourceSetSpecification &spec)
    {
        return new ResourceSetD3D11(spec, this);
    }

    Framebuffer *GraphicsDeviceD3D11::CreateFramebuffer(const FramebufferSpecification &spec)
    {
        return new FramebufferD3D11(spec, m_DevicePtr);
    }

    ID3D11DeviceContext *GraphicsDeviceD3D11::GetDeviceContext()
    {
        return m_DeviceContextPtr;
    }

    std::vector<ID3D11RenderTargetView *> &GraphicsDeviceD3D11::GetActiveRenderTargetViews()
    {
        return m_ActiveRenderTargetviews;
    }

    ID3D11DepthStencilView *&GraphicsDeviceD3D11::GetActiveDepthStencilView()
    {
        return m_ActiveDepthStencilView;
    }
}

#endif