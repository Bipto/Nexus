#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "Nexus/Graphics/TextureFormat.hpp"
#include "Nexus/Graphics/DepthFormat.hpp"

DXGI_FORMAT GetD3D12TextureFormat(Nexus::Graphics::TextureFormat format);
DXGI_FORMAT GetD3D12DepthFormat(Nexus::Graphics::DepthFormat format);

#endif