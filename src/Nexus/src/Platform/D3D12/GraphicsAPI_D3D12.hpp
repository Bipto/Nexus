#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"
	#include "D3D12Include.hpp"

namespace Nexus::Graphics
{
	class GraphicsAPI_D3D12 : public IGraphicsAPI
	{
	  public:
		GraphicsAPI_D3D12(const GraphicsAPICreateInfo &createInfo);
		virtual ~GraphicsAPI_D3D12();
		virtual std::vector<std::shared_ptr<IPhysicalDevice>> GetPhysicalDevices() override;
		virtual Graphics::GraphicsDevice					 *CreateGraphicsDevice(std::shared_ptr<IPhysicalDevice> device) override;
		virtual const GraphicsAPICreateInfo					 &GetGraphicsAPICreateInfo() const override;

	  private:
		GraphicsAPICreateInfo				  m_CreateInfo	= {};
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_DXGIFactory = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Debug5>  m_D3D12Debug	= nullptr;
	};
}	 // namespace Nexus::Graphics

#endif