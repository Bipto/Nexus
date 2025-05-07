#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "GraphicsDeviceD3D12.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
	class TextureD3D12 : public Texture
	{
	  public:
		TextureD3D12(const TextureSpecification &spec, GraphicsDeviceD3D12 *device);
		virtual ~TextureD3D12();
		DXGI_FORMAT			  GetFormat();
		void				  SetResourceState(uint32_t arrayLayer, uint32_t mipLevel, D3D12_RESOURCE_STATES state);
		D3D12_RESOURCE_STATES GetResourceState(uint32_t arrayLayer, uint32_t mipLevel);

		Microsoft::WRL::ComPtr<ID3D12Resource2> GetHandle();

	  private:
		Microsoft::WRL::ComPtr<ID3D12Resource2>		m_Texture		= nullptr;
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> m_Allocation	= nullptr;
		DXGI_FORMAT									m_TextureFormat = DXGI_FORMAT_UNKNOWN;

		TextureSpecification   m_Specification;
		GraphicsDeviceD3D12	  *m_Device = nullptr;

		std::vector<D3D12_RESOURCE_STATES> m_ResourceStates;
	};
}	 // namespace Nexus::Graphics

#endif