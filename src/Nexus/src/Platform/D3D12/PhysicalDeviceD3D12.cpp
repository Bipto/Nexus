#include "PhysicalDeviceD3D12.hpp"

namespace Nexus::Graphics
{
	PhysicalDeviceD3D12::PhysicalDeviceD3D12(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter) : m_Adapter(adapter)
	{
		DXGI_ADAPTER_DESC3 desc;
		adapter->GetDesc3(&desc);

		std::wstring wname(desc.Description);
		m_Name = std::string(wname.begin(), wname.end());

		FindMaximumSupportedFeatureLevel();
	}

	PhysicalDeviceD3D12::~PhysicalDeviceD3D12()
	{
	}

	const std::string &PhysicalDeviceD3D12::GetDeviceName() const
	{
		return m_Name;
	}

	bool PhysicalDeviceD3D12::IsVersionGreaterThan(D3D_FEATURE_LEVEL level)
	{
		if (m_MaxFeatureLevel >= level)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter4> PhysicalDeviceD3D12::GetAdapter() const
	{
		return m_Adapter;
	}

	D3D_FEATURE_LEVEL PhysicalDeviceD3D12::GetMaximumSupportedFeatureLevel() const
	{
		return m_MaxFeatureLevel;
	}

	void PhysicalDeviceD3D12::FindMaximumSupportedFeatureLevel()
	{
		D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_2,
											 D3D_FEATURE_LEVEL_12_1,
											 D3D_FEATURE_LEVEL_12_0,
											 D3D_FEATURE_LEVEL_11_1,
											 D3D_FEATURE_LEVEL_11_0};

		Microsoft::WRL::ComPtr<ID3D12Device> testDevice = nullptr;

		for (auto level : featureLevels)
		{
			if (SUCCEEDED(D3D12CreateDevice(m_Adapter.Get(), level, IID_PPV_ARGS(&testDevice))))
			{
				m_MaxFeatureLevel = level;
				break;
			}
		}
	}
}	 // namespace Nexus::Graphics
