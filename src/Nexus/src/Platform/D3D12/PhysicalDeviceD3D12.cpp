#include "PhysicalDeviceD3D12.hpp"

namespace Nexus::Graphics
{
	PhysicalDeviceD3D12::PhysicalDeviceD3D12(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter) : m_Adapter(adapter)
	{
		DXGI_ADAPTER_DESC3 desc;
		adapter->GetDesc3(&desc);

		std::wstring wname(desc.Description);
		m_Name = std::string(wname.begin(), wname.end());
	}

	PhysicalDeviceD3D12::~PhysicalDeviceD3D12()
	{
	}

	const std::string &PhysicalDeviceD3D12::GetDeviceName() const
	{
		return m_Name;
	}

	const PhysicalDeviceFeatures &PhysicalDeviceD3D12::GetPhysicalDeviceFeatures() const
	{
		return m_Features;
	}

	const PhysicalDeviceLimits &PhysicalDeviceD3D12::GetPhysicalDeviceLimits() const
	{
		return m_Limits;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter4> PhysicalDeviceD3D12::GetAdapter() const
	{
		return m_Adapter;
	}
}	 // namespace Nexus::Graphics
