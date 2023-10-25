#pragma once

#include "Nexus/Graphics/Swapchain.hpp"
#include "Nexus/Window.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "D3D12Include.hpp"

namespace Nexus::Graphics
{
    class SwapchainD3D12 : public Swapchain
    {
    public:
        SwapchainD3D12(Window *window, GraphicsDevice *device, VSyncState vSyncState);
        virtual ~SwapchainD3D12();
        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;

        void Flush();

    private:
        IDXGISwapChain3 *m_Swapchain = nullptr;
        VSyncState m_VsyncState;
        GraphicsDeviceD3D12 *m_Device = nullptr;
    };
}