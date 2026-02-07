#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "Nexus-Core/Graphics/Framebuffer.hpp"
	#include "Nexus-Core/Graphics/Swapchain.hpp"
	#include "Nexus-Core/nxpch.hpp"
	#include "Surface/SurfaceD3D12.hpp"

namespace Nexus::Graphics
{
	class IGraphicsDevice;
	class GraphicsDeviceD3D12;

	class SwapchainD3D12 : public ISwapchain
	{
	  public:
		SwapchainD3D12(IGraphicsDevice *device, ICommandQueue *queue, const SwapchainDescription &swapchainSpec);
		virtual ~SwapchainD3D12();
		void							SwapBuffers(const SwapchainPresentDescription &presentDesc) final;
		Ref<IFramebuffer>				GetCurrentFramebuffer();
		void							SetPresentMode(PresentMode presentMode) final;
		std::pair<uint32_t, uint32_t>	GetSize() final;
		PixelFormat						GetColourFormat() final;
		PixelFormat						GetDepthFormat() final;
		tl::expected<void, std::string> Resize(uint32_t width, uint32_t height) final;

		uint32_t GetCurrentBufferIndex();

		uint32_t GetColorAttachmentCount();

		void AcquireBackbufferIndex();

	  private:
		void Flush();
		void ResizeBuffers();
		void GetBuffers();

		void ReleaseBuffers();
		void CreateFramebuffers();

	  private:
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_Swapchain	   = nullptr;
		GraphicsDeviceD3D12					   *m_Device	   = nullptr;
		ICommandQueue						   *m_CommandQueue = nullptr;

		Ref<SurfaceD3D12> m_Surface = nullptr;

		uint32_t m_SwapchainWidth  = 0;
		uint32_t m_SwapchainHeight = 0;

		uint32_t m_CurrentBufferIndex = 0;

		std::vector<Ref<IFramebuffer>> m_SwapchainFramebuffers = {};
		UINT						   m_SyncInterval		   = 0;
	};
}	 // namespace Nexus::Graphics
#endif