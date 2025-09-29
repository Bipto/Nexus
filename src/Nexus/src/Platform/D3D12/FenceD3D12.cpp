#include "FenceD3D12.hpp"

namespace Nexus::Graphics
{
	FenceD3D12::FenceD3D12(const FenceDescription &desc, GraphicsDeviceD3D12 *device) : m_Description(desc), m_Device(device)
	{
		uint32_t initialValue = 0;
		if (desc.Signalled)
		{
			initialValue = 1;
		}

		Microsoft::WRL::ComPtr<ID3D12Device9> d3d12Device = device->GetD3D12Device();
		HRESULT								  hr		  = d3d12Device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		NX_VALIDATE(SUCCEEDED(hr) == true, "Failed to create fence");

		m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
		hr			 = m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		NX_VALIDATE(SUCCEEDED(hr) == true, "Failed to create fence event");
	}

	FenceD3D12::~FenceD3D12()
	{
		CloseHandle(m_FenceEvent);
	}

	bool FenceD3D12::IsSignalled() const
	{
		return m_Fence->GetCompletedValue() == 1;
	}

	const FenceDescription &FenceD3D12::GetDescription() const
	{
		return m_Description;
	}

	Microsoft::WRL::ComPtr<ID3D12Fence1> FenceD3D12::GetHandle()
	{
		return m_Fence;
	}

	HANDLE FenceD3D12::GetFenceEvent()
	{
		return m_FenceEvent;
	}

	void FenceD3D12::Reset()
	{
		m_Fence->Signal(0);
	}
}	 // namespace Nexus::Graphics
