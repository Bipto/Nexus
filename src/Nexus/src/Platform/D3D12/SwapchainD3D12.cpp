#include "SwapchainD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "FramebufferD3D12.hpp"
	#include "GraphicsDeviceD3D12.hpp"
	#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
	SwapchainD3D12::SwapchainD3D12(IWindow *window, IGraphicsDevice *device, ICommandQueue *queue, const SwapchainDescription &swapchainSpec)
		: ISwapchain(swapchainSpec),
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
			std::vector<RECT> presentRects = {};

			for (const auto &rect : presentDesc.PresentRects)
			{
				RECT &presentRect  = presentRects.emplace_back();
				presentRect.left   = rect.GetLeft();
				presentRect.top	   = rect.GetTop();
				presentRect.right  = rect.GetRight();
				presentRect.bottom = rect.GetBottom();
			}

			DXGI_PRESENT_PARAMETERS presentParams = {};
			presentParams.DirtyRectsCount		  = presentRects.size();
			presentParams.pDirtyRects			  = presentRects.data();
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

	Ref<IFramebuffer> SwapchainD3D12::GetCurrentFramebuffer()
	{
		return m_SwapchainFramebuffers.at(m_CurrentBufferIndex);
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

	void SwapchainD3D12::AcquireBackbufferIndex()
	{
		// retrieve the current buffer index from the swapchain
		m_CurrentBufferIndex = m_Swapchain->GetCurrentBackBufferIndex();
	}

	void SwapchainD3D12::Flush()
	{
		// execute a signal and wait for each buffer in the swapchain
		for (int i = 0; i < BUFFER_COUNT; i++) { m_Device->WaitForIdle(); }

		for (const auto &framebuffer : m_SwapchainFramebuffers)
		{
			Ref<FramebufferD3D12> framebufferD3D12 = std::dynamic_pointer_cast<FramebufferD3D12>(framebuffer);
			framebufferD3D12->Flush();
		}

		m_SwapchainFramebuffers.clear();
	}

	void SwapchainD3D12::RecreateSwapchainIfNecessary()
	{
		const auto &[windowWidth, windowHeight] = m_Window->GetWindowSizeInPixels();

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

		// we need to release the buffers retrieved from the swapchain before new ones
		// can be created
		ReleaseBuffers();

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
		m_SwapchainFramebuffers.clear();
		m_SwapchainFramebuffers.resize(BUFFER_COUNT);
		const auto d3d12Device = m_Device->GetD3D12Device();

		// loop through and retrieve the buffers from the swapchain
		for (size_t i = 0; i < BUFFER_COUNT; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource2> buffer;

			m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer));

			Nexus::Graphics::TextureDescription swapchainTextureDesc = {};
			swapchainTextureDesc.Width								 = m_SwapchainWidth;
			swapchainTextureDesc.Height								 = m_SwapchainHeight;
			swapchainTextureDesc.DepthOrArrayLayers					 = 1;
			swapchainTextureDesc.MipLevels							 = 1;
			swapchainTextureDesc.Samples							 = 1;
			swapchainTextureDesc.Format								 = PixelFormat::R8_G8_B8_A8_UNorm;
			swapchainTextureDesc.Usage								 = Graphics::TextureUsage_ColourAttachment;
			swapchainTextureDesc.DebugName							 = "Swapchain Colour Texture";
			Ref<TextureD3D12> swapchainTexture						 = CreateRef<TextureD3D12>(buffer, swapchainTextureDesc, m_Device);

			Graphics::TextureDescription depthAttachmentDesc = {};
			depthAttachmentDesc.Width						 = m_SwapchainWidth;
			depthAttachmentDesc.Height						 = m_SwapchainHeight;
			depthAttachmentDesc.DepthOrArrayLayers			 = 1;
			depthAttachmentDesc.MipLevels					 = 1;
			depthAttachmentDesc.Samples						 = m_Description.Samples;
			depthAttachmentDesc.Format						 = PixelFormat::D24_UNorm_S8_UInt;
			depthAttachmentDesc.Usage						 = Graphics::TextureUsage_DepthStencilAttachment;
			depthAttachmentDesc.DebugName					 = "Swapchain Depth Texture";
			Ref<TextureD3D12> depthAttachment				 = CreateRef<TextureD3D12>(depthAttachmentDesc, m_Device);

			Graphics::FramebufferTextureSetDescription framebufferDesc = {};

			// create a multisampled framebuffer
			if (m_Description.Samples > 1)
			{
				// create the multisampled texture
				Graphics::TextureDescription multisampledDesc = swapchainTextureDesc;
				multisampledDesc.Samples					  = m_Description.Samples;
				multisampledDesc.DebugName					  = "Swapchain Multisampled Colour Texture";
				Ref<TextureD3D12> multisampledTexture		  = CreateRef<TextureD3D12>(multisampledDesc, m_Device);

				framebufferDesc.ColourAttachments = {FramebufferColourAttachmentDescription {
					.ColourAttachment =
						FramebufferTextureDescription {.BaseArrayLayer = 0, .LayerCount = 1, .MipLevel = 0, .TargetTexture = multisampledTexture},
					.ResolveAttachment =
						FramebufferTextureDescription {.BaseArrayLayer = 0, .LayerCount = 1, .MipLevel = 0, .TargetTexture = swapchainTexture}}};
				framebufferDesc.DepthAttachment	  = {
					  FramebufferTextureDescription {.BaseArrayLayer = 0, .LayerCount = 1, .MipLevel = 0, .TargetTexture = depthAttachment}};

				framebufferDesc.OwnedBySwapchain = true;

				m_SwapchainFramebuffers[i] = m_Device->CreateFramebuffer(framebufferDesc);
			}
			// create a single sampled framebuffer (no need for a resolve attachment)
			else
			{
				framebufferDesc.ColourAttachments = {FramebufferColourAttachmentDescription {
					.ColourAttachment {.BaseArrayLayer = 0, .LayerCount = 1, .MipLevel = 0, .TargetTexture = swapchainTexture}}};
				framebufferDesc.DepthAttachment	  = {
					  FramebufferTextureDescription {.BaseArrayLayer = 0, .LayerCount = 1, .MipLevel = 0, .TargetTexture = depthAttachment}};

				framebufferDesc.OwnedBySwapchain = true;

				m_SwapchainFramebuffers[i] = m_Device->CreateFramebuffer(framebufferDesc);
			}
		}
	}	 // namespace Nexus::Graphics

	void SwapchainD3D12::ReleaseBuffers()
	{
		m_SwapchainFramebuffers.clear();
	}

	void SwapchainD3D12::CreateFramebuffers()
	{
		// retrieve the window's native handle
		NativeWindowInfo info = m_Window->GetNativeWindowInfo();
		HWND			 hwnd = info.hwnd;

		// create the swapchain
		auto windowSize = m_Window->GetWindowSizeInPixels();

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