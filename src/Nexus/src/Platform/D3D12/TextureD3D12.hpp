#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "GraphicsDeviceD3D12.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"

namespace Nexus::Graphics
{
	class Texture2D_D3D12 : public Texture2D
	{
	  public:
		Texture2D_D3D12(GraphicsDeviceD3D12 *device, const Texture2DSpecification &spec);
		virtual ~Texture2D_D3D12();
		virtual void				   SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void GetData(std::vector<unsigned char> &pixels, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		DXGI_FORMAT					   GetFormat();
		const Microsoft::WRL::ComPtr<ID3D12Resource2> &GetD3D12ResourceHandle();

		void				  SetResourceState(uint32_t level, D3D12_RESOURCE_STATES state);
		D3D12_RESOURCE_STATES GetResourceState(uint32_t level);

		bool IsDepth() const
		{
			for (const auto &usage : m_Specification.Usage)
			{
				if (usage == TextureUsage::DepthStencil)
				{
					return true;
				}
			}

			return false;
		}

	  private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_Texture	   = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;

		DXGI_FORMAT m_TextureFormat = DXGI_FORMAT_UNKNOWN;

		Texture2DSpecification m_Specification;
		GraphicsDeviceD3D12	  *m_Device = nullptr;

		std::vector<D3D12_RESOURCE_STATES> m_ResourceStates;
	};

	class Cubemap_D3D12 : public Cubemap
	{
	  public:
		Cubemap_D3D12(const CubemapSpecification &spec, GraphicsDeviceD3D12 *device);
		virtual ~Cubemap_D3D12();

		virtual void SetData(const void *data, CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void GetData(std::vector<unsigned char> &pixels,
							 CubemapFace				 face,
							 uint32_t					 level,
							 uint32_t					 x,
							 uint32_t					 y,
							 uint32_t					 width,
							 uint32_t					 height) override;

		DXGI_FORMAT									   GetFormat();
		const Microsoft::WRL::ComPtr<ID3D12Resource2> &GetD3D12ResourceHandle();

		void				  SetResourceState(uint32_t arrayLayer, uint32_t mipLevel, D3D12_RESOURCE_STATES state);
		D3D12_RESOURCE_STATES GetResourceState(uint32_t arrayLayer, uint32_t mipLevel);

	  private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_Texture	   = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource2> m_UploadBuffer = nullptr;

		DXGI_FORMAT m_TextureFormat = DXGI_FORMAT_UNKNOWN;

		Texture2DSpecification m_Specification;
		GraphicsDeviceD3D12	  *m_Device = nullptr;

		std::vector<std::vector<D3D12_RESOURCE_STATES>> m_ResourceStates;
	};

}	 // namespace Nexus::Graphics

#endif