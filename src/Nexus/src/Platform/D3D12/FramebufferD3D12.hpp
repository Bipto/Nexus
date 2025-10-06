#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "GraphicsDeviceD3D12.hpp"
	#include "Nexus-Core/Graphics/Framebuffer.hpp"

namespace Nexus::Graphics
{
	class TextureD3D12;

	class FramebufferD3D12 : public Framebuffer
	{
	  public:
		FramebufferD3D12(const FramebufferTextureSetDescription &desc, GraphicsDeviceD3D12 *device);
		virtual ~FramebufferD3D12();
		const FramebufferTextureSetDescription GetTextureSetDescription() const final;

		Ref<TextureD3D12> GetD3D12ColourTexture(uint32_t index = 0);
		Ref<TextureD3D12> GetD3D12DepthTexture();

		const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &GetColourAttachmentCPUHandles();
		D3D12_CPU_DESCRIPTOR_HANDLE						GetDepthAttachmentCPUHandle();

	  private:
		void CreateRTVs();
		void AttachTextures();
		void Create();
		void Flush();

	  private:
		GraphicsDeviceD3D12				*m_Device	   = nullptr;
		FramebufferTextureSetDescription m_Description = {};

		// D3D12 resources
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>	 m_ColourAttachmentCPUHandles;
		D3D12_CPU_DESCRIPTOR_HANDLE					 m_DepthAttachmentCPUHandle = {};
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ColorDescriptorHeap		= nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DepthDescriptorHeap		= nullptr;

		std::vector<Ref<TextureD3D12>> m_ColourAttachments;
		Ref<TextureD3D12>			   m_DepthAttachment = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif