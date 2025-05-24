#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "FramebufferD3D12.hpp"
	#include "GraphicsDeviceD3D12.hpp"
	#include "Nexus-Core/Graphics/CommandList.hpp"
	#include "PipelineD3D12.hpp"
	#include "SwapchainD3D12.hpp"

namespace Nexus::Graphics
{
	class CommandListD3D12 : public CommandList
	{
	  public:
		CommandListD3D12(GraphicsDeviceD3D12 *device, const CommandListSpecification &spec);
		virtual ~CommandListD3D12();

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> GetCommandList();

		void Reset();
		void Close();

	  private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	   m_CommandAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CommandList	  = nullptr;
	};
}	 // namespace Nexus::Graphics
#endif