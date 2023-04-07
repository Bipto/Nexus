#include "GraphicsDeviceDX11.h"

#include "backends/imgui_impl_dx11.h"

namespace Nexus
{
    GraphicsDeviceDX11::GraphicsDeviceDX11(Nexus::Window* window, GraphicsAPI api)
        : GraphicsDevice(window, api)
    {
        #if defined(WIN32)
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window->GetSDLWindowHandle(), &wmInfo);
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

        #endif
    }

    void GraphicsDeviceDX11::SetContext()
    {
        m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, NULL);
    }

    void GraphicsDeviceDX11::Clear(float red, float green, float blue, float alpha)
    {
        #if defined(WIN32)
        float backgroundColor[4] = {red, green, blue, alpha};
        m_DeviceContextPtr->ClearRenderTargetView(
            m_RenderTargetViewPtr, backgroundColor  
        );
        #endif
    }

    void GraphicsDeviceDX11::DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader)
    {
        shader->Bind();
        Ref<VertexBufferDX11> vb = std::dynamic_pointer_cast<VertexBufferDX11>(vertexBuffer);

        auto layout = shader->GetLayout();
        ID3D11Buffer* buffer = vb->GetNativeHandle();
        uint32_t stride = layout.GetStride();
        uint32_t offset = 0;

        m_DeviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_DeviceContextPtr->IASetVertexBuffers(
            0,
            1,
            &buffer,
            &stride,
            &offset
        );
        m_DeviceContextPtr->Draw(vertexBuffer->GetVertexCount(), 0);
    }

    void GraphicsDeviceDX11::DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, Ref<Shader> shader)
    {
        shader->Bind();

        Ref<VertexBufferDX11> vb = std::dynamic_pointer_cast<VertexBufferDX11>(vertexBuffer);
        Ref<IndexBufferDX11> ib = std::dynamic_pointer_cast<IndexBufferDX11>(indexBuffer);

        auto layout = shader->GetLayout();
        ID3D11Buffer* dx11VertexBuffer = vb->GetNativeHandle();
        ID3D11Buffer* dx11IndexBuffer = ib->GetNativeHandle();
        uint32_t stride = layout.GetStride();
        uint32_t offset = 0;

        m_DeviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_DeviceContextPtr->IASetVertexBuffers(
            0,
            1,
            &dx11VertexBuffer,
            &stride,
            &offset
        );
        m_DeviceContextPtr->IASetIndexBuffer(
            dx11IndexBuffer,
            DXGI_FORMAT_R32_UINT,
            0
        );

        m_DeviceContextPtr->DrawIndexed(indexBuffer->GetIndexCount(), 0, 0);
    }

    const char* GraphicsDeviceDX11::GetAPIName()
    {
        return "DirectX11";
    }

    const char* GraphicsDeviceDX11::GetDeviceName()
    {
        return "Test";
    }

    void* GraphicsDeviceDX11::GetContext()
    {
        return m_DeviceContextPtr;
    }

    Ref<Shader> GraphicsDeviceDX11::CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout)
    {
        return {};
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

    Ref<Texture> GraphicsDeviceDX11::CreateTexture(const char* filepath)
    {
        return {};
    }

    Ref<Framebuffer> GraphicsDeviceDX11::CreateFramebuffer(const Nexus::FramebufferSpecification& spec)
    {
        return {};
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
        hr = m_SwapChainPtr->ResizeBuffers(0, size.Width, size.Height, DXGI_FORMAT_UNKNOWN, 0);

        ID3D11Texture2D* pBuffer;
        hr = m_SwapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
        hr = m_DevicePtr->CreateRenderTargetView(pBuffer, NULL, &m_RenderTargetViewPtr);
        pBuffer->Release();

        m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, NULL);

        D3D11_VIEWPORT vp;
        vp.Width = (float)size.Width;
        vp.Height = (float)size.Height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_DeviceContextPtr->RSSetViewports(1, &vp);

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
    }
}