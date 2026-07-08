#pragma once

#include <expected>

#include "RHI/ISurface.hpp"

#include "../D3D12Include.hpp"
#include "RHI/SwapchainDescription.hpp"

namespace Nexus::Graphics
{
    class SurfaceD3D12 : public ISurface
    {
      public:
        virtual ~SurfaceD3D12() = default;
        virtual std::expected<Microsoft::WRL::ComPtr<IDXGISwapChain1>, std::string> CreateDXGISwapchain(
            const SwapchainDescription &swapchainDesc, ID3D12CommandQueue *commandQueue, IDXGIFactory2 *factory
        ) const = 0;
    };
} // namespace Nexus::Graphics