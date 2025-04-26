#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/DeviceBuffer.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceD3D12;

	class DeviceBufferD3D12 : public DeviceBuffer
	{
	  public:
		DeviceBufferD3D12(const DeviceBufferDescription &desc, GraphicsDeviceD3D12 *graphicsDevice);
		virtual ~DeviceBufferD3D12();
		virtual void						   SetData(const void *data, uint32_t offset, uint32_t size) final;
		virtual std::vector<char>			   GetData(uint32_t offset, uint32_t size) const final;
		virtual const DeviceBufferDescription &GetDescription() const final;

		Microsoft::WRL::ComPtr<ID3D12Resource2> GetHandle();

	  private:
		DeviceBufferDescription						m_BufferDescription = {};
		Microsoft::WRL::ComPtr<ID3D12Resource2>		m_BufferHandle		= nullptr;
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> m_Allocation		= nullptr;
	};
}	 // namespace Nexus::Graphics

#endif