#include "GraphicsDeviceDX11.h"

#include "backends/imgui_impl_dx11.h"

#include "Core/Logging/Log.h"

namespace Nexus
{
    GraphicsDeviceDX11::GraphicsDeviceDX11(const GraphicsDeviceCreateInfo& createInfo)
        : GraphicsDevice(createInfo)
    {
        #if defined(WIN32)
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(m_Window->GetSDLWindowHandle(), &wmInfo);
        HWND hwnd = wmInfo.info.win.window;

        DXGI_SWAP_CHAIN_DESC swap_chain_desc = {0};
        swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
        swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
        swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = 1;
        swap_chain_desc.OutputWindow = hwnd;
        swap_chain_desc.Windowed = true;

        D3D_FEATURE_LEVEL feature_level;
        UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
        #if defined(DEBUG) | defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            flags,
            NULL,
            0,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &m_SwapChainPtr,
            &m_DevicePtr,
            &feature_level,
            &m_DeviceContextPtr
        );

        ID3D11Texture2D* framebuffer;
        hr = m_SwapChainPtr->GetBuffer(
            0,
            _uuidof(ID3D11Texture2D),
            (void**)&framebuffer 
        );

        hr = m_DevicePtr->CreateRenderTargetView(
            framebuffer, 0, &m_RenderTargetViewPtr
        );

        framebuffer->Release();
        m_ActiveRenderTargetviews = {m_RenderTargetViewPtr};

        IDXGIFactory1* factory;
        IDXGIAdapter1* adapter;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));
        factory->EnumAdapters1(0, &adapter);
        DXGI_ADAPTER_DESC1 adapterDesc;
        adapter->GetDesc1(&adapterDesc);
        std::wstring ws(adapterDesc.Description);      
        m_AdapterName = std::string(ws.begin(), ws.end());

        ID3D11RasterizerState* noCull;
        D3D11_RASTERIZER_DESC rastDesc;
        ZeroMemory(&rastDesc, sizeof(rastDesc));
        rastDesc.FillMode = D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_NONE;
        m_DevicePtr->CreateRasterizerState(&rastDesc, &noCull);
        m_DeviceContextPtr->RSSetState(noCull);

        
        #endif
    }

    void GraphicsDeviceDX11::SetContext()
    {
        m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, NULL);
    }

    void GraphicsDeviceDX11::Clear(float red, float green, float blue, float alpha)
    {
        #if defined(WIN32)

        for (auto target : m_ActiveRenderTargetviews)
        {
            float backgroundColor[4] = { red, green, blue, alpha };
            m_DeviceContextPtr->ClearRenderTargetView(
                target, backgroundColor
            );
        }

        #endif
    }

    void GraphicsDeviceDX11::SetFramebuffer(Ref<Framebuffer> framebuffer)
    {
        if (framebuffer)
        {
            Ref<FramebufferDX11> dxFramebuffer = std::dynamic_pointer_cast<FramebufferDX11>(framebuffer);
    
            std::vector<ID3D11RenderTargetView*> colorTargets;
            for (const auto& colorTarget : dxFramebuffer->GetColorRenderTargets())
            {
                colorTargets.push_back(colorTarget.RenderTargetView);
            }
            ID3D11DepthStencilView* depthTarget = NULL;

            if (dxFramebuffer->HasDepthTexture())
            {
                depthTarget = dxFramebuffer->GetDepthRenderTarget().DepthStencilView;
            }

            m_ActiveRenderTargetviews = colorTargets;
        }
        else
        {
            m_ActiveRenderTargetviews = { m_RenderTargetViewPtr };
        }

        m_DeviceContextPtr->OMSetRenderTargets(
            m_ActiveRenderTargetviews.size(),
            m_ActiveRenderTargetviews.data(),
            NULL
        );
    }

    void GraphicsDeviceDX11::DrawElements(PrimitiveType type, uint32_t start, uint32_t count)
    {
        m_DeviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_DeviceContextPtr->Draw(count, start);
    }

    void GraphicsDeviceDX11::DrawIndexed(PrimitiveType type, uint32_t count, uint32_t offset)
    {        
        m_DeviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);       
        m_DeviceContextPtr->DrawIndexed(count, offset, 0);
    }

    void GraphicsDeviceDX11::SetViewport(const Viewport &viewport)
    {
        m_Viewport = viewport;

        D3D11_VIEWPORT vp;
        vp.Width = (float)viewport.Width;
        vp.Height = (float)viewport.Height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = viewport.X;
        vp.TopLeftY = viewport.Y;
        m_DeviceContextPtr->RSSetViewports(1, &vp);
    }

    const Viewport &GraphicsDeviceDX11::GetViewport()
    {
        return m_Viewport;
    }

    const char* GraphicsDeviceDX11::GetAPIName()
    {
        return "DirectX11";
    }

    const char* GraphicsDeviceDX11::GetDeviceName()
    {
        return m_AdapterName.c_str();
    }

    void* GraphicsDeviceDX11::GetContext()
    {
        return m_DeviceContextPtr;
    }

    void GraphicsDeviceDX11::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        if (!m_ActiveShader)
        {
            NX_ERROR("No shader is currently bound");
            return;
        }
        
        auto layout = m_ActiveShader->GetLayout();
        
        uint32_t stride = layout.GetStride();
        uint32_t offset = 0;

        Ref<VertexBufferDX11> vb = std::dynamic_pointer_cast<VertexBufferDX11>(vertexBuffer);
        ID3D11Buffer* dx11VertexBuffer = vb->GetNativeHandle();

        m_DeviceContextPtr->IASetVertexBuffers(
            0,
            1,
            &dx11VertexBuffer,
            &stride,
            &offset
        );
    }

    void GraphicsDeviceDX11::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        Ref<IndexBufferDX11> ib = std::dynamic_pointer_cast<IndexBufferDX11>(indexBuffer);
        ID3D11Buffer* dx11IndexBuffer = ib->GetNativeHandle();

        m_DeviceContextPtr->IASetIndexBuffer(
            dx11IndexBuffer,
            DXGI_FORMAT_R32_UINT,
            0
        );
    }

    void GraphicsDeviceDX11::SetShader(Ref<Shader> shader)
    {
        shader->Bind();
        m_ActiveShader = shader;
    }

    Ref<Shader> GraphicsDeviceDX11::CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout)
    {
        return CreateRef<ShaderDX11>(m_DevicePtr, m_DeviceContextPtr, vertexShaderSource, fragmentShaderSource, layout);
    }

    Ref<Shader> GraphicsDeviceDX11::CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout)
    {
        return CreateRef<ShaderDX11>(m_DevicePtr, m_DeviceContextPtr, filepath, layout); 
    }

    Ref<VertexBuffer> GraphicsDeviceDX11::CreateVertexBuffer(const std::vector<float> vertices)
    {
        return CreateRef<VertexBufferDX11>(m_DevicePtr, vertices);
    }

    Ref<IndexBuffer> GraphicsDeviceDX11::CreateIndexBuffer(const std::vector<unsigned int> indices)
    {
        return CreateRef<IndexBufferDX11>(m_DevicePtr, indices);
    }

    Ref<Texture> GraphicsDeviceDX11::CreateTexture(TextureSpecification spec)
    {
        return CreateRef<TextureDX11>(m_DevicePtr, spec);
    }

    Ref<UniformBuffer> GraphicsDeviceDX11::CreateUniformBuffer(uint32_t size, uint32_t binding)
    {
        return CreateRef<UniformBufferDX11>(
            m_DevicePtr,
            m_DeviceContextPtr,
            size,
            binding);
    }

    Ref<Framebuffer> GraphicsDeviceDX11::CreateFramebuffer(const Nexus::FramebufferSpecification& spec)
    {
        return CreateRef<FramebufferDX11>(m_DevicePtr, spec);
    }

    void GraphicsDeviceDX11::InitialiseImGui()
    {
        ImGui_ImplDX11_Init(m_DevicePtr, m_DeviceContextPtr);
    }

    void GraphicsDeviceDX11::BeginImGuiRender()
    {
        ImGui_ImplDX11_NewFrame();
    }

    void GraphicsDeviceDX11::EndImGuiRender()
    {
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void GraphicsDeviceDX11::Resize(Point size)
    {
        #if defined(WIN32)

        m_DeviceContextPtr->OMSetRenderTargets(0, 0, 0);
        m_RenderTargetViewPtr->Release();

        HRESULT hr;        
        hr = m_SwapChainPtr->ResizeBuffers(0, size.X, size.Y, DXGI_FORMAT_UNKNOWN, 0);


        ID3D11Texture2D* pBuffer;
        hr = m_SwapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
        hr = m_DevicePtr->CreateRenderTargetView(pBuffer, NULL, &m_RenderTargetViewPtr);
        pBuffer->Release();

        m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, NULL);

        #endif
    }

    void GraphicsDeviceDX11::SwapBuffers()
    {
        #if defined(WIN32)
        m_SwapChainPtr->Present(m_VsyncValue, 0);
        #endif
    }

    void GraphicsDeviceDX11::SetVSyncState(VSyncState vSyncState)
    {
        #if defined(WIN32)
                switch (vSyncState)
                {
                    case VSyncState::Enabled:
                        m_VsyncValue = 1;
                        break;
                    default:
                        m_VsyncValue = 0;
                        break;
                }
        #endif

        m_VsyncState = vSyncState;
    }

    VSyncState GraphicsDeviceDX11::GetVsyncState()
    {
        return m_VsyncState;
    }
}