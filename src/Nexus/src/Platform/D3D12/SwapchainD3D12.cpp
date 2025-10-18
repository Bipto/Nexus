#include "SwapchainD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "GraphicsDeviceD3D12.hpp"
	#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
	SwapchainD3D12::SwapchainD3D12(IWindow *window, GraphicsDevice *device, ICommandQueue *queue, const SwapchainDescription &swapchainSpec)
		: Swapchain(swapchainSpec),
		  m_CommandQueue(queue),
		  m_Window(window)
	{
		// assign the graphics device
		m_Device = (GraphicsDeviceD3D12 *)device;

		// get the sync interval for the swapchain
		m_SyncInterval = D3D12::GetSyncIntervalFromPresentMode(m_Description.ImagePresentMode);

		// set up size of swapchain
		Point2D<uint32_t> windowSize = m_Window->GetWindowSizeInPixels();
		m_SwapchainWidth			 = windowSize.X;
		m_SwapchainHeight			 = windowSize.Y;

		// setup framebuffers for swapchain images
		CreateFramebuffers();
		AcquireBackbufferIndex();
	}

	Nexus::Graphics::SwapchainD3D12::~SwapchainD3D12()
	{
		// we need to flush the swapchain to ensure that resources are not in use when
		// we attempt to delete them
		Flush();

		m_Device->WaitForIdle();
	}

	void SwapchainD3D12::SwapBuffers(const SwapchainPresentDescription &presentDesc)
	{
		// swap the swapchain's buffers and present to the display
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain1;
		HRESULT									hr			 = m_Swapchain->QueryInterface(IID_PPV_ARGS(&swapchain1));
		UINT									presentFlags = 0;

		if (SUCCEEDED(hr))
		{
			DXGI_PRESENT_PARAMETERS presentParams = {};
			presentParams.DirtyRectsCount		  = 0;
			presentParams.pDirtyRects			  = nullptr;
			presentParams.pScrollOffset			  = nullptr;
			presentParams.pScrollRect			  = nullptr;

			swapchain1->Present1(m_SyncInterval, presentFlags, &presentParams);
		}
		else
		{
			m_Swapchain->Present(m_SyncInterval, presentFlags);
		}

		// recreate the swapchain if the window's size has changed
		RecreateSwapchainIfNecessary();

		AcquireBackbufferIndex();
	}

	Ref<Framebuffer> SwapchainD3D12::GetCurrentFramebuffer()
	{
		return nullptr;
	}

	void SwapchainD3D12::SetPresentMode(PresentMode presentMode)
	{
		m_Description.ImagePresentMode = presentMode;
		m_SyncInterval				   = D3D12::GetSyncIntervalFromPresentMode(presentMode);
	}

	Nexus::Point2D<uint32_t> SwapchainD3D12::GetSize()
	{
		return {m_SwapchainWidth, m_SwapchainHeight};
	}

	PixelFormat SwapchainD3D12::GetColourFormat()
	{
		return PixelFormat::R8_G8_B8_A8_UNorm;
	}

	PixelFormat SwapchainD3D12::GetDepthFormat()
	{
		return PixelFormat::D24_UNorm_S8_UInt;
	}

	uint32_t SwapchainD3D12::GetCurrentBufferIndex()
	{
		return m_CurrentBufferIndex;
	}

	uint32_t SwapchainD3D12::GetColorAttachmentCount()
	{
		return 1;
	}

	bool SwapchainD3D12::HasMultisampledFramebuffer() const
	{
		return m_Description.Samples > 1;
	}

	void SwapchainD3D12::AcquireBackbufferIndex()
	{
		// retrieve the current buffer index from the swapchain
		m_CurrentBufferIndex = m_Swapchain->GetCurrentBackBufferIndex();
	}

	void SwapchainD3D12::Flush()
	{
		// execute a signal and wait for each buffer in the swapchain
		for (int i = 0; i < BUFFER_COUNT; i++) { m_Device->WaitForIdle(); }
	}

	void SwapchainD3D12::RecreateSwapchainIfNecessary()
	{
		auto windowWidth  = m_Window->GetWindowSize().X;
		auto windowHeight = m_Window->GetWindowSize().Y;

		// if the size of the window is the same, we do not need to do anything and
		// can return
		if (m_SwapchainWidth == windowWidth && m_SwapchainHeight == windowHeight)
			return;

		m_SwapchainWidth  = windowWidth;
		m_SwapchainHeight = windowHeight;

		// resize the swapchain
		ResizeBuffers();
	}

	void SwapchainD3D12::ResizeBuffers()
	{
		// flush swapchain to ensure that buffers are not in use
		Flush();

		// resize the swapchains buffers
		m_Swapchain->ResizeBuffers(BUFFER_COUNT,
								   m_SwapchainWidth,
								   m_SwapchainHeight,
								   DXGI_FORMAT_UNKNOWN,
								   DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

		// retrieve the new buffers
		GetBuffers();
	}

	void SwapchainD3D12::GetBuffers()
	{
		const auto d3d12Device = m_Device->GetD3D12Device();

		// loop through and retrieve the buffers from the swapchain
		for (size_t i = 0; i < BUFFER_COUNT; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource2> buffer;

			m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer));

			Nexus::Graphics::TextureDescription desc = {};
			desc.Width								 = m_SwapchainWidth;
			desc.Height								 = m_SwapchainHeight;
			desc.DepthOrArrayLayers					 = 1;
			desc.MipLevels							 = 1;
			desc.Format								 = PixelFormat::R8_G8_B8_A8_UNorm;
			Ref<TextureD3D12> texture				 = CreateRef<TextureD3D12>(buffer, desc, m_Device);
		}
	}

	void SwapchainD3D12::CreateFramebuffers()
	{
		// retrieve the window's native handle
		NativeWindowInfo info = m_Window->GetNativeWindowInfo();
		HWND			 hwnd = info.hwnd;

		// create the swapchain
		auto windowSize = m_Window->GetWindowSize();

		// set up properties for the swapchain
		DXGI_SWAP_CHAIN_DESC1 swapchainDesc {};
		swapchainDesc.Width				 = windowSize.X;
		swapchainDesc.Height			 = windowSize.Y;
		swapchainDesc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo			 = false;
		swapchainDesc.SampleDesc.Count	 = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage		 = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount		 = BUFFER_COUNT;
		swapchainDesc.Scaling			 = DXGI_SCALING_STRETCH;
		swapchainDesc.SwapEffect		 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.AlphaMode			 = DXGI_ALPHA_MODE_IGNORE;
		swapchainDesc.Flags				 = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		// create a fullscreen description, we will not use exclusive fullscreen so we
		// don't need this
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc {};
		fullscreenDesc.Windowed = true;

		// retrieve the graphics device's DXGI factory
		auto factory = m_Device->GetDXGIFactory();

		CommandQueueD3D12 *commandQueueD3D12 = (CommandQueueD3D12 *)m_CommandQueue;

		// create the swapchain and query for the correct swapchain type
		Microsoft::WRL::ComPtr<IDXGISwapChain1>	   sc1;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = commandQueueD3D12->GetHandle();
		factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapchainDesc, &fullscreenDesc, nullptr, sc1.GetAddressOf());
		if (SUCCEEDED(sc1->QueryInterface(IID_PPV_ARGS(&m_Swapchain)))) {}

		// retrieve the ID3D12Device
		const auto d3d12Device = m_Device->GetD3D12Device();

		// get the buffers from the swapchain
		GetBuffers();
	}
}	 // namespace Nexus::Graphics
#endif