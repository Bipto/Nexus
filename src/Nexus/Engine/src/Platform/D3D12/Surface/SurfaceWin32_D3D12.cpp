#pragma once

#include "SurfaceWin32_D3D12.hpp"

namespace Nexus::Graphics
{
    SurfaceWin32_D3D12::SurfaceWin32_D3D12(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance)
        : m_Hwnd(hwnd), m_Hdc(hdc), m_Hinstance(hinstance)
    {
    }

    std::expected<Microsoft::WRL::ComPtr<IDXGISwapChain1>, std::string> SurfaceWin32_D3D12::CreateDXGISwapchain(
        const SwapchainDescription &swapchainDesc, ID3D12CommandQueue *commandQueue, IDXGIFactory2 *factory
    ) const
    {
        // set up properties for the swapchain
        DXGI_SWAP_CHAIN_DESC1 dxgiSwapchainDesc{};
        dxgiSwapchainDesc.Width = swapchainDesc.Width;
        dxgiSwapchainDesc.Height = swapchainDesc.Height;
        dxgiSwapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dxgiSwapchainDesc.Stereo = false;
        dxgiSwapchainDesc.SampleDesc.Count = 1;
        dxgiSwapchainDesc.SampleDesc.Quality = 0;
        dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dxgiSwapchainDesc.BufferCount = BUFFER_COUNT;
        dxgiSwapchainDesc.Scaling = DXGI_SCALING_STRETCH;
        dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        dxgiSwapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        dxgiSwapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        // create a fullscreen description, we will not use exclusive fullscreen so
        // we don't need this
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc{};
        fullscreenDesc.Windowed = true;

        // create the swapchain and query for the correct swapchain type
        Microsoft::WRL::ComPtr<IDXGISwapChain1> sc1;
        HRESULT hr = factory->CreateSwapChainForHwnd(
            commandQueue, reinterpret_cast<HWND>(m_Hwnd), &dxgiSwapchainDesc, &fullscreenDesc, nullptr,
            sc1.GetAddressOf()
        );

        if (FAILED(hr))
        {
            _com_error err(hr);
            return std::unexpected(err.ErrorMessage());
        }

        return sc1;
    }

    const uintptr_t SurfaceWin32_D3D12::GetHwnd() const
    {
        return m_Hwnd;
    }

    const uintptr_t SurfaceWin32_D3D12::GetHdc() const
    {
        return m_Hdc;
    }

    const uintptr_t SurfaceWin32_D3D12::GetHinstance() const
    {
        return m_Hinstance;
    }
} // namespace Nexus::Graphics