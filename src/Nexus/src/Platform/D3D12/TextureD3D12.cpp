#include "TextureD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Utils.hpp"
	#include "Nexus-Core/Utils/Utils.hpp"

namespace Nexus::Graphics
{
	TextureD3D12::TextureD3D12(const TextureSpecification &spec, GraphicsDeviceD3D12 *device) : Texture(spec), m_Device(device)
	{
		NX_ASSERT(spec.ArrayLayers >= 1, "Texture must have at least one array layer");
		NX_ASSERT(spec.MipLevels >= 1, "Texture must have at least one mip level");

		if (spec.Usage & TextureUsage_Cubemap)
		{
			NX_ASSERT(spec.ArrayLayers % 6 == 0, "Cubemap texture must have 6 array layers");
		}

		if (spec.Samples > 1)
		{
			NX_ASSERT(spec.MipLevels == 1, "Multisampled textures do not support mipmapping");
		}

		bool isDepth = (spec.Usage & TextureUsage_DepthStencil) != 0;

		D3D12_RESOURCE_DIMENSION dimension = D3D12::GetResourceDimensions(spec.Type);
		D3D12_RESOURCE_FLAGS	 flags	   = D3D12::GetResourceFlags(spec.Usage);
		m_TextureFormat					   = D3D12::GetD3D12PixelFormat(spec.Format, isDepth);

		D3D12_RESOURCE_DESC1 textureDesc = {};
		textureDesc.Dimension			 = dimension;
		textureDesc.Alignment			 = 0;
		textureDesc.Width				 = spec.Width;
		textureDesc.Height				 = spec.Height;
		textureDesc.DepthOrArraySize	 = spec.ArrayLayers;
		textureDesc.MipLevels			 = spec.MipLevels;
		textureDesc.Format				 = m_TextureFormat;
		textureDesc.SampleDesc.Count	 = spec.Samples;
		textureDesc.SampleDesc.Quality	 = 0;
		textureDesc.Layout				 = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Flags				 = flags;

		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType					= D3D12_HEAP_TYPE_DEFAULT;

		Microsoft::WRL::ComPtr<D3D12MA::Allocator> allocator = device->GetAllocator();
		HRESULT									   hr =
			allocator->CreateResource2(&allocationDesc, &textureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, &m_Allocation, IID_PPV_ARGS(&m_Texture));

		if (FAILED(hr))
		{
			_com_error	error(hr);
			std::string errorMessage = std::string("Failed to create texture: ") + error.ErrorMessage();
			throw std::runtime_error(errorMessage.c_str());
		}

		for (uint32_t arrayLayer = 0; arrayLayer < spec.ArrayLayers; arrayLayer++)
		{
			for (uint32_t mipLevel = 0; mipLevel < spec.MipLevels; mipLevel++) { m_ResourceStates.push_back(D3D12_RESOURCE_STATE_COMMON); }
		}
	}

	TextureD3D12::~TextureD3D12()
	{
	}

	DXGI_FORMAT TextureD3D12::GetFormat()
	{
		return m_TextureFormat;
	}

	void TextureD3D12::SetResourceState(uint32_t arrayLayer, uint32_t mipLevel, D3D12_RESOURCE_STATES state)
	{
		/* NX_ASSERT(arrayLayer <= m_Specification.ArrayLayers, "Array layer is greater than the total number of array layers");
		NX_ASSERT(mipLevel <= m_Specification.MipLevels, "Mip level is greater than the total number of mip levels"); */

		m_ResourceStates[arrayLayer * m_Specification.MipLevels + mipLevel] = state;
	}

	D3D12_RESOURCE_STATES TextureD3D12::GetResourceState(uint32_t arrayLayer, uint32_t mipLevel)
	{
		/* NX_ASSERT(arrayLayer <= m_Specification.ArrayLayers, "Array layer is greater than the total number of array layers");
		NX_ASSERT(mipLevel <= m_Specification.MipLevels, "Mip level is greater than the total number of mip levels"); */

		return m_ResourceStates[arrayLayer * m_Specification.MipLevels + mipLevel];
	}

	Microsoft::WRL::ComPtr<ID3D12Resource2> TextureD3D12::GetHandle()
	{
		return m_Texture;
	}
}	 // namespace Nexus::Graphics

#endif