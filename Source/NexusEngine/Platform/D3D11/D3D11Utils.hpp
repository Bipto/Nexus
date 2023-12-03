#pragma once

#if defined(NX_PLATFORM_D3D11)

#include "D3D11Include.hpp"
#include "Nexus/Graphics/TextureFormat.hpp"
#include "Nexus/Graphics/DepthFormat.hpp"

DXGI_FORMAT GetD3D11TextureFormat(Nexus::Graphics::TextureFormat format);
DXGI_FORMAT GetD3D11DepthFormat(Nexus::Graphics::DepthFormat format);

#endif