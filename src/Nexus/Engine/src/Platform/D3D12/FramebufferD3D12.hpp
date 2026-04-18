#pragma once

#if defined(NX_PLATFORM_D3D12)

	#include "D3D12Include.hpp"
	#include "GraphicsDeviceD3D12.hpp"
	#include "RHI/Framebuffer.hpp"

namespace Nexus::Graphics
{
	class TextureD3D12;

	class FramebufferD3D12 : public IFramebuffer
	{
	  public:
		FramebufferD3D12(const FramebufferTextureSetDescription &desc, GraphicsDeviceD3D12 *device);
		virtual ~FramebufferD3D12();
		const FramebufferTextureSetDescription GetTextureSetDescription() const final;

		const TextureD3D12 *GetD3D12ColourTexture(uint32_t index = 0);
		const TextureD3D12 *GetD3D12DepthTexture();

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

		std::vector<TextureD3D12 *> m_ColourAttachments;
		std::vector<TextureD3D12 *> m_ResolveAttachments;
		TextureD3D12			   *m_DepthAttachment = nullptr;

		friend class SwapchainD3D12;
	};
}	 // namespace Nexus::Graphics

#endif