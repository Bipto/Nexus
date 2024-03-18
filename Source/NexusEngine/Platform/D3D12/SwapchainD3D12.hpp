#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "Nexus/Graphics/Swapchain.hpp"
#include "Nexus/Window.hpp"
#include "GraphicsDeviceD3D12.hpp"
#include "D3D12Include.hpp"

#include "Nexus/Graphics/Framebuffer.hpp"

#include <array>

namespace Nexus::Graphics
{
    class SwapchainD3D12 : public Swapchain
    {
    public:
        SwapchainD3D12(Window *window, GraphicsDevice *device, const SwapchainSpecification &swapchainSpec);
        virtual ~SwapchainD3D12();
        virtual void Initialise() override{};
        virtual void SwapBuffers() override;
        virtual VSyncState GetVsyncState() override;
        virtual void SetVSyncState(VSyncState vsyncState) override;
        virtual Window *GetWindow() override { return m_Window; }
        virtual void Prepare() override;
        Microsoft::WRL::ComPtr<ID3D12Resource2> RetrieveBufferHandle();
        uint32_t GetCurrentBufferIndex();

        const D3D12_CPU_DESCRIPTOR_HANDLE RetrieveRenderTargetViewDescriptorHandle() const;

        ID3D12Resource2 *RetrieveDepthBufferHandle();
        D3D12_CPU_DESCRIPTOR_HANDLE RetrieveDepthBufferDescriptorHandle();

        uint32_t GetColorAttachmentCount();
        const D3D12_RESOURCE_STATES GetCurrentTextureState() const;
        const D3D12_RESOURCE_STATES GetCurrentDepthState() const;
        void SetTextureState(D3D12_RESOURCE_STATES state);
        void SetDepthState(D3D12_RESOURCE_STATES state);
        Ref<Framebuffer> GetMultisampledFramebuffer();

    private:
        void Flush();
        void RecreateSwapchainIfNecessary();
        void ResizeBuffers();
        void GetBuffers();
        void ReleaseBuffers();

        void CreateColourAttachments();
        void CreateDepthAttachment();
        void CreateMultisampledFramebuffer();

    private:
        Window *m_Window = nullptr;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_Swapchain = nullptr;
        VSyncState m_VsyncState;
        GraphicsDeviceD3D12 *m_Device = nullptr;

        uint32_t m_SwapchainWidth = 0;
        uint32_t m_SwapchainHeight = 0;

        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource2>> m_Buffers;
        uint32_t m_CurrentBufferIndex = 0;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RenderTargetViewDescriptorHeap = nullptr;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RenderTargetViewDescriptorHandles;

        Microsoft::WRL::ComPtr<ID3D12Resource2> m_DepthBuffer = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DepthTextureDescriptorHeap = nullptr;

        std::vector<D3D12_RESOURCE_STATES> m_CurrentTextureStates;
        D3D12_RESOURCE_STATES m_CurrentDepthState;

        Nexus::Ref<Framebuffer> m_MultisampledFramebuffer = nullptr;
    };
}
#endif