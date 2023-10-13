#pragma once

#if defined(NX_PLATFORM_DX11)
#include "DX11.hpp"
#include "Nexus/Graphics/Swapchain.hpp"
#include "Nexus/Window.hpp"

namespace Nexus::Graphics
{
    // forward declaration
    class GraphicsDeviceDX11;

    class SwapchainDX11 : public Swapchain
    {
    public:
        SwapchainDX11(Window *window, GraphicsDeviceDX11 *device, IDXGISwapChain *swapchain, VSyncState vSyncState);
        ~SwapchainDX11();

        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;

        ID3D11RenderTargetView *GetRenderTargetView();
        ID3D11DepthStencilView *GetDepthStencilView();

    private:
        void RecreateSwapchainIfNecessary();
        void ResizeBuffers(uint32_t width, uint32_t height);

    private:
        IDXGISwapChain *m_Swapchain = NULL;
        ID3D11RenderTargetView *m_RenderTargetView = NULL;

        ID3D11Texture2D *m_SwapchainDepthTexture = NULL;
        ID3D11DepthStencilView *m_SwapchainDepthTextureView = NULL;

        VSyncState m_VsyncState;
        GraphicsDeviceDX11 *m_Device;
        Window *m_Window;

        uint32_t m_SwapchainWidth = 0;
        uint32_t m_SwapchainHeight = 0;
    };
}
#endif