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
        SwapchainDX11(Nexus::Window *window, ID3D11Device *device, IDXGISwapChain *swapchain, VSyncState vSyncState);
        ~SwapchainDX11();

        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        void Resize(uint32_t width, uint32_t height);

        ID3D11RenderTargetView *GetRenderTargetView();
        ID3D11DepthStencilView *GetDepthStencilView();

    private:
        IDXGISwapChain *m_Swapchain = NULL;
        ID3D11RenderTargetView *m_RenderTargetView = NULL;

        ID3D11Texture2D *m_SwapchainDepthTexture = NULL;
        ID3D11DepthStencilView *m_SwapchainDepthTextureView = NULL;

        VSyncState m_VsyncState;
        ID3D11Device *m_Device;
    };
}
#endif