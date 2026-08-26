#pragma once

#include <cstdint>

#include "SurfaceD3D12.hpp"

namespace Nexus::Graphics
{
    class SurfaceWin32_D3D12 final : public SurfaceD3D12
    {
      public:
        SurfaceWin32_D3D12(uintptr_t hwnd, uintptr_t hdc, uintptr_t hinstance);
        virtual ~SurfaceWin32_D3D12() final = default;

        std::expected<Microsoft::WRL::ComPtr<IDXGISwapChain1>, std::string> CreateDXGISwapchain(
            const SwapchainDescription &swapchainDesc, ID3D12CommandQueue *commandQueue,
            IDXGIFactory2 *factory) const final;

        const uintptr_t GetHwnd() const;
        const uintptr_t GetHdc() const;
        const uintptr_t GetHinstance() const;

      private:
        uintptr_t m_Hwnd = 0;
        uintptr_t m_Hdc = 0;
        uintptr_t m_Hinstance = 0;
    };
} // namespace Nexus::Graphics