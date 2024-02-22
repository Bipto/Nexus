#pragma once

#if defined(NX_PLATFORM_D3D12)

#include "D3D12Include.hpp"
#include "Nexus/Graphics/TextureFormat.hpp"
#include "Nexus/Graphics/DepthFormat.hpp"
#include "Nexus/Graphics/SamplerState.hpp"
#include "Nexus/Vertex.hpp"

DXGI_FORMAT GetD3D12TextureFormat(Nexus::Graphics::TextureFormat format);
DXGI_FORMAT GetD3D12DepthFormat(Nexus::Graphics::DepthFormat format);

DXGI_FORMAT GetD3D12BaseType(const Nexus::Graphics::VertexBufferElement &element);
D3D12_CULL_MODE GetCullMode(Nexus::Graphics::CullMode cullMode);
D3D12_COMPARISON_FUNC GetComparisonFunction(Nexus::Graphics::ComparisonFunction function);
D3D12_STENCIL_OP GetStencilOperation(Nexus::Graphics::StencilOperation operation);
D3D12_FILL_MODE GetFillMode(Nexus::Graphics::FillMode fillMode);
D3D12_BLEND GetBlendFunction(Nexus::Graphics::BlendFunction function);
D3D12_BLEND_OP GetBlendEquation(Nexus::Graphics::BlendEquation equation);

D3D12_FILTER GetD3D12Filter(Nexus::Graphics::SamplerFilter filter);
D3D12_TEXTURE_ADDRESS_MODE GetD3D12TextureAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);

#endif