#pragma once

#if defined(NX_PLATFORM_D3D11)
#include "D3D11Include.hpp"
#include "Nexus/Graphics/Swapchain.hpp"
#include "Nexus/Window.hpp"

namespace Nexus::Graphics
{
    // forward declaration
    class GraphicsDeviceD3D11;

    class SwapchainD3D11 : public Swapchain
    {
    public:
        SwapchainD3D11(Window *window, GraphicsDevice *device, VSyncState vSyncState);
        ~SwapchainD3D11();

        virtual void Initialise() override {}
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
        GraphicsDeviceD3D11 *m_Device;
        Window *m_Window;

        uint32_t m_SwapchainWidth = 0;
        uint32_t m_SwapchainHeight = 0;
    };
}
#endif