#pragma once

#if defined(NX_PLATFORM_DX11)
#include "DX11.hpp"
#include "Nexus/Graphics/Swapchain.hpp"
#include "Nexus/Window.hpp"

namespace Nexus::Graphics
{
    class SwapchainDX11 : public Swapchain
    {
    public:
        SwapchainDX11::SwapchainDX11(Window *window, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain, VSyncState vSyncState);
        virtual ~SwapchainDX11();

        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        void Resize(uint32_t width, uint32_t height);

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView();
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthStencilView();

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_Swapchain = NULL;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView = NULL;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_SwapchainDepthTexture = NULL;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_SwapchainDepthTextureView = NULL;

        VSyncState m_VsyncState;
        Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    };
}
#endif