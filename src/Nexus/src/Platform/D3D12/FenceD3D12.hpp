#pragma once

#include "Nexus-Core/Graphics/Fence.hpp"
#include "Platform/D3D12/D3D12Include.hpp"

#include "GraphicsDeviceD3D12.hpp"

namespace Nexus::Graphics
{
	class FenceD3D12 : public Fence
	{
	  public:
		FenceD3D12(const FenceDescription &desc, GraphicsDeviceD3D12 *device);
		virtual ~FenceD3D12();

		bool					IsSignalled() const final;
		const FenceDescription &GetDescription() const final;

		ID3D12Fence1 *GetHandle();
		HANDLE		  GetFenceEvent();

		void Reset();

	  private:
		FenceDescription					 m_Description = {};
		GraphicsDeviceD3D12					*m_Device	   = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence	   = nullptr;
		uint64_t							 m_FenceValue  = 0;
		HANDLE								 m_FenceEvent  = nullptr;
	};
}	 // namespace Nexus::Graphics