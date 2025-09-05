#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/Framebuffer.hpp"
	#include "Nexus-Core/Graphics/Swapchain.hpp"
	#include "Nexus-Core/IWindow.hpp"
	#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class GraphicsDevice;
	class GraphicsDeviceD3D12;

	class SwapchainD3D12 : public Swapchain
	{
	  public:
		SwapchainD3D12(IWindow *window, GraphicsDevice *device, const SwapchainDescription &swapchainSpec);
		virtual ~SwapchainD3D12();
		void SwapBuffers();
		void SetPresentMode(PresentMode presentMode) final;

		IWindow *GetWindow() final
		{
			return m_Window;
		}
		Nexus::Point2D<uint32_t> GetSize() final;
		PixelFormat				 GetColourFormat() final;
		PixelFormat				 GetDepthFormat() final;

		Microsoft::WRL::ComPtr<ID3D12Resource2> RetrieveBufferHandle();
		uint32_t								GetCurrentBufferIndex();

		const D3D12_CPU_DESCRIPTOR_HANDLE RetrieveRenderTargetViewDescriptorHandle() const;

		Microsoft::WRL::ComPtr<ID3D12Resource2> RetrieveDepthBufferHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE				RetrieveDepthBufferDescriptorHandle();

		uint32_t					GetColorAttachmentCount();
		const D3D12_RESOURCE_STATES GetCurrentTextureState() const;
		const D3D12_RESOURCE_STATES GetCurrentDepthState() const;
		void						SetTextureState(D3D12_RESOURCE_STATES state);
		void						SetDepthState(D3D12_RESOURCE_STATES state);
		Ref<Framebuffer>			GetMultisampledFramebuffer();

		void AcquireBackbufferIndex();

	  private:
		void Flush();
		void RecreateSwapchainIfNecessary();
		void ResizeBuffers();
		void GetBuffers();
		void ReleaseBuffers();

		void CreateColourAttachments();
		void CreateDepthAttachment();
		void CreateMultisampledFramebuffer();

		void Resolve();

	  private:
		IWindow								   *m_Window	= nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_Swapchain = nullptr;
		GraphicsDeviceD3D12					   *m_Device	= nullptr;

		uint32_t m_SwapchainWidth  = 0;
		uint32_t m_SwapchainHeight = 0;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource2>> m_Buffers;
		uint32_t											 m_CurrentBufferIndex			  = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		 m_RenderTargetViewDescriptorHeap = nullptr;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>			 m_RenderTargetViewDescriptorHandles;

		Microsoft::WRL::ComPtr<ID3D12Resource2>		 m_DepthBuffer				  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DepthTextureDescriptorHeap = nullptr;

		std::vector<D3D12_RESOURCE_STATES> m_CurrentTextureStates;
		D3D12_RESOURCE_STATES			   m_CurrentDepthState;

		Nexus::Ref<Framebuffer> m_MultisampledFramebuffer = nullptr;
		UINT					m_SyncInterval			  = 0;
	};
}	 // namespace Nexus::Graphics
#endif