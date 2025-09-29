#include "FramebufferD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "D3D12Utils.hpp"
	#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
	FramebufferD3D12::FramebufferD3D12(const FramebufferSpecification &spec, GraphicsDeviceD3D12 *device) : Framebuffer(spec), m_Device(device)
	{
		if (spec.Width != 0 && spec.Height != 0)
		{
			Recreate();
		}
	}

	FramebufferD3D12::~FramebufferD3D12()
	{
		Flush();
	}

	void FramebufferD3D12::SetFramebufferSpecification(const FramebufferSpecification &spec)
	{
		m_Description = spec;

		m_ColorDescriptorHeap = nullptr;
		m_DepthDescriptorHeap = nullptr;
		m_ColorAttachmentCPUHandles.clear();
		m_DepthAttachmentCPUHandle = {};
		m_ColorAttachments.clear();
		m_DepthAttachment = nullptr;

		Flush();
		Recreate();
	}

	Ref<Texture> FramebufferD3D12::GetColorTexture(uint32_t index)
	{
		return m_ColorAttachments.at(index);
	}

	Ref<Texture> FramebufferD3D12::GetDepthTexture()
	{
		return m_DepthAttachment;
	}

	Ref<TextureD3D12> FramebufferD3D12::GetD3D12ColorTexture(uint32_t index)
	{
		return m_ColorAttachments.at(index);
	}

	Ref<TextureD3D12> FramebufferD3D12::GetD3D12DepthTexture()
	{
		return m_DepthAttachment;
	}

	const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &FramebufferD3D12::GetColorAttachmentCPUHandles()
	{
		return m_ColorAttachmentCPUHandles;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FramebufferD3D12::GetDepthAttachmentCPUHandle()
	{
		return m_DepthAttachmentCPUHandle;
	}

	void FramebufferD3D12::Recreate()
	{
		auto d3d12Device = m_Device->GetD3D12Device();

		CreateAttachments();
		CreateRTVs();
	}

	void FramebufferD3D12::Flush()
	{
		for (int i = 0; i < BUFFER_COUNT; i++) { m_Device->WaitForIdle(); }
	}

	const FramebufferSpecification FramebufferD3D12::GetFramebufferSpecification()
	{
		return m_Description;
	}

	void FramebufferD3D12::CreateAttachments()
	{
		for (int i = 0; i < m_Description.ColourAttachmentSpecification.Attachments.size(); i++)
		{
			const auto &colorAttachmentSpec = m_Description.ColourAttachmentSpecification.Attachments.at(i);

			NX_VALIDATE(GetPixelFormatType(colorAttachmentSpec.TextureFormat) == Graphics::PixelFormatType::Colour,
						"Depth attachment must have a valid colour format");

			Graphics::TextureDescription spec = {};
			spec.Width						  = m_Description.Width;
			spec.Height						  = m_Description.Height;
			spec.Format						  = colorAttachmentSpec.TextureFormat;
			spec.Samples					  = m_Description.Samples;
			spec.Usage						  = Graphics::TextureUsage_Sampled | Graphics::TextureUsage_RenderTarget;

			Ref<Texture> texture = Ref<Texture>(m_Device->CreateTexture(spec));
			m_ColorAttachments.push_back(std::dynamic_pointer_cast<TextureD3D12>(texture));
		}

		if (m_Description.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid)
		{
			NX_VALIDATE(GetPixelFormatType(m_Description.DepthAttachmentSpecification.DepthFormat) == Graphics::PixelFormatType::DepthStencil,
						"Depth attachment must have a depth format");

			Graphics::TextureDescription spec = {};
			spec.Width						  = m_Description.Width;
			spec.Height						  = m_Description.Height;
			spec.Format						  = m_Description.DepthAttachmentSpecification.DepthFormat;
			spec.Usage						  = 0;
			spec.Samples					  = m_Description.Samples;

			Ref<Texture> texture = Ref<Texture>(m_Device->CreateTexture(spec));
			m_DepthAttachment	 = std::dynamic_pointer_cast<TextureD3D12>(texture);
		}
	}

	void FramebufferD3D12::CreateRTVs()
	{
		auto d3d12Device = m_Device->GetD3D12Device();

		// create descriptor heaps
		D3D12_DESCRIPTOR_HEAP_DESC colorDescriptorHeapDesc;
		colorDescriptorHeapDesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		colorDescriptorHeapDesc.NumDescriptors = m_Description.ColourAttachmentSpecification.Attachments.size();
		colorDescriptorHeapDesc.NodeMask	   = 0;
		colorDescriptorHeapDesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		d3d12Device->CreateDescriptorHeap(&colorDescriptorHeapDesc, IID_PPV_ARGS(&m_ColorDescriptorHeap));

		D3D12_DESCRIPTOR_HEAP_DESC depthDescriptorHeapDesc;
		depthDescriptorHeapDesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		depthDescriptorHeapDesc.NumDescriptors = 1;
		depthDescriptorHeapDesc.NodeMask	   = 0;
		depthDescriptorHeapDesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		d3d12Device->CreateDescriptorHeap(&depthDescriptorHeapDesc, IID_PPV_ARGS(&m_DepthDescriptorHeap));

		// retrieve descriptor handles
		auto cpuHandle = m_ColorDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (uint32_t i = 0; i < m_Description.ColourAttachmentSpecification.Attachments.size(); i++)
		{
			auto texture = m_ColorAttachments.at(i);
			m_ColorAttachmentCPUHandles.push_back(cpuHandle);

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = D3D12::GetD3D12PixelFormat(m_Description.ColourAttachmentSpecification.Attachments[i].TextureFormat);

			if (m_Description.Samples > 1)
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			}

			rtvDesc.Texture2D.MipSlice	 = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			auto resourceHandle = texture->GetHandle();
			d3d12Device->CreateRenderTargetView(resourceHandle.Get(), &rtvDesc, cpuHandle);

			auto incrementSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			cpuHandle.ptr += incrementSize;
		}

		m_DepthAttachmentCPUHandle = m_DepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		if (m_Description.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid)
		{
			auto texture = m_DepthAttachment;

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Format = D3D12::GetD3D12PixelFormat(m_Description.DepthAttachmentSpecification.DepthFormat);

			if (m_Description.Samples > 1)
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			}

			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.Flags			   = D3D12_DSV_FLAG_NONE;

			auto resourceHandle = texture->GetHandle();
			d3d12Device->CreateDepthStencilView(resourceHandle.Get(), &dsvDesc, m_DepthAttachmentCPUHandle);
		}
	}
}	 // namespace Nexus::Graphics

#endif