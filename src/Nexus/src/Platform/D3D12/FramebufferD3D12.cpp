#include "FramebufferD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "D3D12Utils.hpp"
	#include "TextureD3D12.hpp"

namespace Nexus::Graphics
{
	FramebufferD3D12::FramebufferD3D12(const FramebufferTextureSetDescription &desc, GraphicsDeviceD3D12 *device)
		: m_Device(device),
		  m_Description(desc)
	{
		NX_VALIDATE(desc.ValidateSamples(), "Sample count must match across all textures in a framebuffer");
		NX_VALIDATE(desc.ValidateDimensions(), "The dimensions of all textures in a framebuffer must match");
		NX_VALIDATE(desc.ValidateUsageFlags(), "The usage flags of all textures must be correct for usage in a framebuffer");

		Create();
	}

	FramebufferD3D12::~FramebufferD3D12()
	{
		Flush();
	}

	const FramebufferTextureSetDescription FramebufferD3D12::GetTextureSetDescription() const
	{
		return m_Description;
	}

	Ref<TextureD3D12> FramebufferD3D12::GetD3D12ColourTexture(uint32_t index)
	{
		return m_ColourAttachments.at(index);
	}

	Ref<TextureD3D12> FramebufferD3D12::GetD3D12DepthTexture()
	{
		return m_DepthAttachment;
	}

	const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &FramebufferD3D12::GetColourAttachmentCPUHandles()
	{
		return m_ColourAttachmentCPUHandles;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FramebufferD3D12::GetDepthAttachmentCPUHandle()
	{
		return m_DepthAttachmentCPUHandle;
	}

	void FramebufferD3D12::Create()
	{
		auto d3d12Device = m_Device->GetD3D12Device();
		AttachTextures();
		CreateRTVs();
	}

	void FramebufferD3D12::Flush()
	{
		for (int i = 0; i < BUFFER_COUNT; i++) { m_Device->WaitForIdle(); }
	}

	void FramebufferD3D12::CreateRTVs()
	{
		auto d3d12Device = m_Device->GetD3D12Device();

		// create colour attachment descriptor heap if needed
		if (m_Description.ColourAttachments.size() > 0)
		{
			D3D12_DESCRIPTOR_HEAP_DESC colorDescriptorHeapDesc = {};
			colorDescriptorHeapDesc.Type					   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			colorDescriptorHeapDesc.NumDescriptors			   = m_Description.ColourAttachments.size();
			colorDescriptorHeapDesc.NodeMask				   = 0;
			colorDescriptorHeapDesc.Flags					   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			d3d12Device->CreateDescriptorHeap(&colorDescriptorHeapDesc, IID_PPV_ARGS(&m_ColorDescriptorHeap));
		}

		// create depth/stencil heap if needed
		if (m_Description.DepthAttachment.has_value())
		{
			D3D12_DESCRIPTOR_HEAP_DESC depthDescriptorHeapDesc = {};
			depthDescriptorHeapDesc.Type					   = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			depthDescriptorHeapDesc.NumDescriptors			   = 1;
			depthDescriptorHeapDesc.NodeMask				   = 0;
			depthDescriptorHeapDesc.Flags					   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			d3d12Device->CreateDescriptorHeap(&depthDescriptorHeapDesc, IID_PPV_ARGS(&m_DepthDescriptorHeap));
		}

		// retrieve sample count
		uint32_t sampleCount = m_Description.GetSampleCount();

		// retrieve descriptor handles
		auto cpuHandle = m_ColorDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (uint32_t i = 0; i < m_Description.ColourAttachments.size(); i++)
		{
			Ref<TextureD3D12> textureD3D12 = m_ColourAttachments.at(i);
			Ref<Texture>	  texture	   = textureD3D12;
			m_ColourAttachmentCPUHandles.push_back(cpuHandle);

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

			rtvDesc.Format = D3D12::GetD3D12PixelFormat(textureD3D12->GetPixelFormat());

			if (sampleCount > 1)
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			}

			rtvDesc.Texture2D.MipSlice	 = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			auto resourceHandle = textureD3D12->GetHandle();
			d3d12Device->CreateRenderTargetView(resourceHandle.Get(), &rtvDesc, cpuHandle);

			auto incrementSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			cpuHandle.ptr += incrementSize;
		}

		if (m_Description.DepthAttachment.has_value())
		{
			m_DepthAttachmentCPUHandle = m_DepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			auto &texture = m_DepthAttachment;

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format						  = D3D12::GetD3D12PixelFormat(texture->GetPixelFormat());

			if (sampleCount > 1)
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

	void FramebufferD3D12::AttachTextures()
	{
		m_ColourAttachments.clear();
		m_ColourAttachmentCPUHandles.clear();

		for (const auto &colourAttachment : m_Description.ColourAttachments)
		{
			m_ColourAttachments.push_back(std::dynamic_pointer_cast<TextureD3D12>(colourAttachment.ColourAttachment.TargetTexture));
		}

		if (m_Description.DepthAttachment.has_value())
		{
			m_DepthAttachment = std::dynamic_pointer_cast<TextureD3D12>(m_Description.DepthAttachment.value().TargetTexture);
		}
	}
}	 // namespace Nexus::Graphics

#endif