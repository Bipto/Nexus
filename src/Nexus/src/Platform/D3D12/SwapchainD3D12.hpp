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
		SwapchainD3D12(IWindow *window, GraphicsDevice *device, ICommandQueue *queue, const SwapchainDescription &swapchainSpec);
		virtual ~SwapchainD3D12();
		void			 SwapBuffers() final;
		Ref<Framebuffer> GetCurrentFramebuffer();
		void			 SetPresentMode(PresentMode presentMode) final;

		IWindow *GetWindow() final
		{
			return m_Window;
		}
		Nexus::Point2D<uint32_t> GetSize() final;
		PixelFormat				 GetColourFormat() final;
		PixelFormat				 GetDepthFormat() final;

		uint32_t GetCurrentBufferIndex();

		uint32_t GetColorAttachmentCount();

		bool HasMultisampledFramebuffer() const;

		void AcquireBackbufferIndex();

	  private:
		void Flush();
		void RecreateSwapchainIfNecessary();
		void ResizeBuffers();
		void GetBuffers();

		void CreateFramebuffers();

	  private:
		IWindow								   *m_Window	   = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_Swapchain	   = nullptr;
		GraphicsDeviceD3D12					   *m_Device	   = nullptr;
		ICommandQueue						   *m_CommandQueue = nullptr;

		uint32_t m_SwapchainWidth  = 0;
		uint32_t m_SwapchainHeight = 0;

		uint32_t m_CurrentBufferIndex = 0;

		std::vector<Ref<Framebuffer>> m_SwapchainFramebuffers = {};
		UINT						  m_SyncInterval		  = 0;
	};
}	 // namespace Nexus::Graphics
#endif