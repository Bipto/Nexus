#include "SwapchainDX11.hpp"

#if defined(NX_PLATFORM_DX11)

namespace Nexus::Graphics
{
    SwapchainDX11::SwapchainDX11(Window *window, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain, VSyncState vSyncState)
    {
        m_Swapchain = swapchain;
        m_VsyncState = vSyncState;
        m_Device = device;

        // create view into swapchain
        {
            Microsoft::WRL::ComPtr<ID3D11Texture2D> framebuffer;
            HRESULT hr = m_Swapchain->GetBuffer(
                0,
                _uuidof(ID3D11Texture2D),
                (void **)framebuffer.GetAddressOf());

            hr = m_Device->CreateRenderTargetView(
                framebuffer.Get(), 0, &m_RenderTargetView);
        }

        // create depthstencil attachment for swapchain
        {
            D3D11_TEXTURE2D_DESC depthDesc;
            ZeroMemory(&depthDesc, sizeof(depthDesc));
            depthDesc.Width = window->GetWindowSize().X;
            depthDesc.Height = window->GetWindowSize().Y;
            depthDesc.MipLevels = 1;
            depthDesc.ArraySize = 1;
            depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthDesc.SampleDesc.Count = 1;
            depthDesc.SampleDesc.Quality = 0;
            depthDesc.Usage = D3D11_USAGE_DEFAULT;
            depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            depthDesc.CPUAccessFlags = 0;
            depthDesc.MiscFlags = 0;

            HRESULT hr = m_Device->CreateTexture2D(
                &depthDesc,
                NULL,
                &m_SwapchainDepthTexture);

            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.Format = depthDesc.Format;
            depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            depthStencilDesc.Texture2D.MipSlice = 0;

            hr = m_Device->CreateDepthStencilView(
                m_SwapchainDepthTexture.Get(),
                &depthStencilDesc,
                m_SwapchainDepthTextureView.GetAddressOf());
        }
    }

    SwapchainDX11::~SwapchainDX11()
    {
    }

    void SwapchainDX11::SwapBuffers()
    {
        m_Swapchain->Present((uint32_t)m_VsyncState, 0);
    }

    VSyncState SwapchainDX11::GetVsyncState()
    {
        return m_VsyncState;
    }

    void SwapchainDX11::SetVSyncState(VSyncState vsyncState)
    {
        m_VsyncState = vsyncState;
    }

    void SwapchainDX11::Resize(uint32_t width, uint32_t height)
    {
        HRESULT hr = m_Swapchain->ResizeBuffers(
            0,
            width,
            height,
            DXGI_FORMAT_UNKNOWN,
            0);

        if (FAILED(hr))
        {
            _com_error error(hr);
            NX_ERROR(error.ErrorMessage());
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> pBuffer;
        hr = m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)pBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            _com_error error(hr);
            NX_ERROR(error.ErrorMessage());
        }

        hr = m_Device->CreateRenderTargetView(
            pBuffer.Get(),
            NULL,
            &m_RenderTargetView);

        D3D11_TEXTURE2D_DESC depthDesc;
        ZeroMemory(&depthDesc, sizeof(depthDesc));
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthDesc.CPUAccessFlags = 0;
        depthDesc.MiscFlags = 0;

        hr = m_Device->CreateTexture2D(&depthDesc, NULL, &m_SwapchainDepthTexture);
        if (FAILED(hr))
        {
            _com_error error(hr);
            NX_ERROR(error.ErrorMessage());
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
        depthStencilDesc.Format = depthDesc.Format;
        depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        depthStencilDesc.Texture1D.MipSlice = 0;

        hr = m_Device->CreateDepthStencilView(m_SwapchainDepthTexture.Get(), &depthStencilDesc, &m_SwapchainDepthTextureView);
        if (FAILED(hr))
        {
            _com_error error(hr);
            NX_ERROR(error.ErrorMessage());
        }
    }

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> SwapchainDX11::GetRenderTargetView()
    {
        return m_RenderTargetView;
    }

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> SwapchainDX11::GetDepthStencilView()
    {
        return m_SwapchainDepthTextureView;
    }
}

#endif