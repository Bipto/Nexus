#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "Nexus-Core/Graphics/IPhysicalDevice.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "Platform/D3D12/D3D12Include.hpp"

namespace Nexus::Graphics
{
	class PhysicalDeviceD3D12 final : public IPhysicalDevice
	{
	  public:
		PhysicalDeviceD3D12(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);
		virtual ~PhysicalDeviceD3D12();
		const std::string					 &GetDeviceName() const final;
		virtual const PhysicalDeviceFeatures &GetPhysicalDeviceFeatures() const final;
		virtual const PhysicalDeviceLimits	 &GetPhysicalDeviceLimits() const final;

		Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter() const;
		D3D_FEATURE_LEVEL					  GetMaximumSupportedFeatureLevel() const;

	  private:
		void FindMaximumSupportedFeatureLevel();

	  private:
		std::string							  m_Name	 = {};
		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_Adapter	 = nullptr;
		PhysicalDeviceFeatures				  m_Features = {};
		PhysicalDeviceLimits				  m_Limits	 = {};
		D3D_FEATURE_LEVEL					  m_MaxFeatureLevel = D3D_FEATURE_LEVEL_10_0;
	};
}	 // namespace Nexus::Graphics

#endif