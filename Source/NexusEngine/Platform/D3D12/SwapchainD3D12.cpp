#include "SwapchainD3D12.hpp"

#include "SDL_syswm.h"

namespace Nexus::Graphics
{
    SwapchainD3D12::SwapchainD3D12(Window *window, GraphicsDevice *device, VSyncState vSyncState)
        : m_VsyncState(vSyncState)
    {
        m_Device = (GraphicsDeviceD3D12 *)device;

        // retrieve the window's native handle
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window->GetSDLWindowHandle(), &wmInfo);
        HWND hwnd = wmInfo.info.win.window;

        // create the swapchain
        auto windowSize = window->GetWindowSize();

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

        auto factory = m_Device->GetDXGIFactory();

        IDXGISwapChain1 *sc1;
        factory->CreateSwapChainForHwnd(m_Device->GetCommandQueue(), hwnd, &swapchainDesc, &fullscreenDesc, nullptr, &sc1);
        if (SUCCEEDED(sc1->QueryInterface(IID_PPV_ARGS(&m_Swapchain))))
        {
            sc1->Release();
        }
    }

    Nexus::Graphics::SwapchainD3D12::~SwapchainD3D12()
    {
        Flush();
        m_Swapchain->Release();
    }

    void SwapchainD3D12::SwapBuffers()
    {
        m_Swapchain->Present((uint32_t)m_VsyncState, 0);
    }

    VSyncState SwapchainD3D12::GetVsyncState()
    {
        return m_VsyncState;
    }

    void SwapchainD3D12::SetVSyncState(VSyncState vsyncState)
    {
        m_VsyncState = vsyncState;
    }

    void SwapchainD3D12::Flush()
    {
        for (int i = 0; i < BUFFER_COUNT; i++)
        {
            m_Device->SignalAndWait();
        }
    }
}