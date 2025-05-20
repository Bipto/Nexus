#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/PixelFormat.hpp"
	#include "Nexus-Core/Graphics/SamplerState.hpp"
	#include "Nexus-Core/Graphics/ShaderModule.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"
	#include "Nexus-Core/Vertex.hpp"

namespace Nexus::D3D12
{
	DXGI_FORMAT GetD3D12PixelFormat(Nexus::Graphics::PixelFormat format, bool isDepth);

	DXGI_FORMAT
	GetD3D12BaseType(const Nexus::Graphics::VertexBufferElement &element);
	D3D12_CULL_MODE GetCullMode(Nexus::Graphics::CullMode cullMode);
	D3D12_COMPARISON_FUNC
	GetComparisonFunction(Nexus::Graphics::ComparisonFunction function);
	D3D12_STENCIL_OP
	GetStencilOperation(Nexus::Graphics::StencilOperation operation);
	D3D12_FILL_MODE GetFillMode(Nexus::Graphics::FillMode fillMode);
	D3D12_BLEND		GetBlendFunction(Nexus::Graphics::BlendFactor function);
	D3D12_BLEND_OP	GetBlendEquation(Nexus::Graphics::BlendEquation equation);

	D3D12_FILTER GetD3D12Filter(Nexus::Graphics::SamplerFilter filter);
	D3D12_TEXTURE_ADDRESS_MODE
	GetD3D12TextureAddressMode(Nexus::Graphics::SamplerAddressMode addressMode);

	DXGI_FORMAT
	GetD3D12IndexBufferFormat(Nexus::Graphics::IndexBufferFormat format);
	D3D12_PRIMITIVE_TOPOLOGY_TYPE
	GetPipelineTopology(Nexus::Graphics::Topology topology);

	D3D12_HEAP_TYPE GetHeapType(const Graphics::DeviceBufferDescription &desc);
	D3D12_RESOURCE_DIMENSION GetResourceDimensions(Nexus::Graphics::TextureType textureType);
	D3D12_RESOURCE_FLAGS	 GetResourceFlags(uint8_t textureUsage);

	D3D12_BARRIER_SYNC	 GetBarrierSyncType(Nexus::Graphics::BarrierStage stage);
	D3D12_BARRIER_ACCESS GetBarrierAccessType(Nexus::Graphics::BarrierAccess access);
	D3D12_BARRIER_LAYOUT GetBarrierLayout(Nexus::Graphics::BarrierLayout layout);

	D3D12_SHADER_RESOURCE_VIEW_DESC CreateTextureSrvView(const Graphics::TextureSpecification &spec);
	D3D12_UNORDERED_ACCESS_VIEW_DESC CreateTextureUavView(const Graphics::StorageImageView &view);

}	 // namespace Nexus::D3D12

#endif