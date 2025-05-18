#if defined(NX_PLATFORM_D3D12)

	#include "GraphicsAPI_D3D12.hpp"
	#include "PhysicalDeviceD3D12.hpp"
	#include "GraphicsDeviceD3D12.hpp"

namespace Nexus::Graphics
{
	void ReportLiveObjects()
	{
		// initialise dxgi debug layer
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			OutputDebugStringW(L"Reporting live D3D12 objects:\n");
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}

	GraphicsAPI_D3D12::GraphicsAPI_D3D12(const GraphicsAPICreateInfo &createInfo) : m_CreateInfo(createInfo)
	{
		UUID experimentalFeatures[] = {D3D12ExperimentalShaderModels};
		D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

		if (createInfo.Debug)
		{
			NX_ASSERT(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12Debug))), "Could not create a D3D12 Debug Interface");
			m_D3D12Debug->EnableDebugLayer();
			std::atexit(ReportLiveObjects);
		}

		NX_ASSERT(SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DXGIFactory))), "Failed to create DXGI Factory");
	}

	GraphicsAPI_D3D12::~GraphicsAPI_D3D12()
	{
	}

	std::vector<std::shared_ptr<IPhysicalDevice>> GraphicsAPI_D3D12::GetPhysicalDevices()
	{
		std::vector<std::shared_ptr<IPhysicalDevice>> physicalDevices;
		Microsoft::WRL::ComPtr<IDXGIAdapter4>		  adapter = nullptr;
		// for (UINT i = 0; m_DXGIFactory->EnumAdapters(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND, i++) {}

		for (UINT i = 0;
			 m_DXGIFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
			 ++i)
		{
			DXGI_ADAPTER_DESC3 desc;
			adapter->GetDesc3(&desc);

			if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
			{
				physicalDevices.push_back(std::make_shared<PhysicalDeviceD3D12>(adapter));
			}
		}

		return physicalDevices;
	}

	Graphics::GraphicsDevice *GraphicsAPI_D3D12::CreateGraphicsDevice(std::shared_ptr<IPhysicalDevice> device)
	{
		return new Graphics::GraphicsDeviceD3D12(device, m_DXGIFactory);
	}

	const GraphicsAPICreateInfo &GraphicsAPI_D3D12::GetGraphicsAPICreateInfo() const
	{
		return m_CreateInfo;
	}
}	 // namespace Nexus::Graphics

#endif