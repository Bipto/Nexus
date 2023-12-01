#include "SwapchainD3D11.hpp"

#if defined(NX_PLATFORM_D3D11)

#include "Platform/D3D11/GraphicsDeviceD3D11.hpp"

#include "SDL_syswm.h"

#include <dxgi1_6.h>

namespace Nexus::Graphics
{
    SwapchainD3D11::SwapchainD3D11(Window *window, GraphicsDevice *device, VSyncState vSyncState)
        : m_VsyncState(vSyncState), m_Window(window)
    {
        m_Device = (GraphicsDeviceD3D11 *)device;
        auto D3D11Device = m_Device->GetDevice();

        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(m_Window->GetSDLWindowHandle(), &wmInfo);
        HWND hwnd = wmInfo.info.win.window;

        IDXGIFactory2 *factory;
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        if (FAILED(hr))
        {
            NX_ERROR("Failed to create DXGIFactory");
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
        swapChainDesc.BufferDesc.Width = window->GetWindowSize().X;
        swapChainDesc.BufferDesc.Height = window->GetWindowSize().Y;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 3;
        swapChainDesc.OutputWindow = hwnd;
        swapChainDesc.Windowed = true;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

        factory->CreateSwapChain(D3D11Device, &swapChainDesc, &m_Swapchain);

        // create view into swapchain
        {
            ID3D11Texture2D *framebuffer;
            HRESULT hr = m_Swapchain->GetBuffer(
                0,
                _uuidof(ID3D11Texture2D),
                (void **)&framebuffer);

            hr = D3D11Device->CreateRenderTargetView(
                framebuffer, 0, &m_RenderTargetView);

            framebuffer->Release();
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

            HRESULT hr = D3D11Device->CreateTexture2D(
                &depthDesc,
                NULL,
                &m_SwapchainDepthTexture);

            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.Format = depthDesc.Format;
            depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            depthStencilDesc.Texture2D.MipSlice = 0;

            hr = D3D11Device->CreateDepthStencilView(
                m_SwapchainDepthTexture,
                &depthStencilDesc,
                &m_SwapchainDepthTextureView);
        }
    }

    SwapchainD3D11::~SwapchainD3D11()
    {
        m_SwapchainDepthTextureView->Release();
        m_SwapchainDepthTexture->Release();
        m_RenderTargetView->Release();
        m_Swapchain->Release();
    }

    void SwapchainD3D11::SwapBuffers()
    {
        m_Swapchain->Present((uint32_t)m_VsyncState, 0);
        RecreateSwapchainIfNecessary();
    }

    VSyncState SwapchainD3D11::GetVsyncState()
    {
        return m_VsyncState;
    }

    void SwapchainD3D11::SetVSyncState(VSyncState vsyncState)
    {
        m_VsyncState = vsyncState;
    }

    void SwapchainD3D11::Prepare()
    {
    }

    void SwapchainD3D11::ResizeBuffers(uint32_t width, uint32_t height)
    {
        auto D3D11Device = m_Device->GetDevice();

        m_RenderTargetView->Release();
        m_SwapchainDepthTexture->Release();
        m_SwapchainDepthTextureView->Release();

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

        ID3D11Texture2D *pBuffer;
        hr = m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBuffer);
        if (FAILED(hr))
        {
            _com_error error(hr);
            NX_ERROR(error.ErrorMessage());
        }

        hr = D3D11Device->CreateRenderTargetView(
            pBuffer,
            NULL,
            &m_RenderTargetView);

        pBuffer->Release();

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

        hr = D3D11Device->CreateTexture2D(&depthDesc, NULL, &m_SwapchainDepthTexture);
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

        hr = D3D11Device->CreateDepthStencilView(m_SwapchainDepthTexture, &depthStencilDesc, &m_SwapchainDepthTextureView);
        if (FAILED(hr))
        {
            _com_error error(hr);
            NX_ERROR(error.ErrorMessage());
        }
    }
    ID3D11RenderTargetView *SwapchainD3D11::GetRenderTargetView()
    {
        return m_RenderTargetView;
    }

    ID3D11DepthStencilView *SwapchainD3D11::GetDepthStencilView()
    {
        return m_SwapchainDepthTextureView;
    }

    void SwapchainD3D11::RecreateSwapchainIfNecessary()
    {
        auto windowWidth = m_Window->GetWindowSize().X;
        auto windowHeight = m_Window->GetWindowSize().Y;

        if (m_SwapchainWidth == windowWidth && m_SwapchainHeight == windowHeight)
            return;

        auto contextPtr = m_Device->GetDeviceContext();
        contextPtr->OMSetRenderTargets(0, 0, 0);

        ResizeBuffers(windowWidth, windowHeight);

        m_SwapchainWidth = windowWidth;
        m_SwapchainHeight = windowHeight;
    }
}

#endif