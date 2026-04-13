#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "RHI/Framebuffer.hpp"
	#include "TextureVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class FramebufferVk : public IFramebuffer
	{
	  public:
		FramebufferVk(const FramebufferTextureSetDescription &desc, GraphicsDeviceVk *device);
		~FramebufferVk();

		const FramebufferTextureSetDescription GetTextureSetDescription() const final;

		const TextureVk *GetVulkanColourTexture(uint32_t index = 0);
		const TextureVk *GetVulkanDepthTexture();
		const TextureVk *GetVulkanResolveTexture(uint32_t index = 0);

		VkRenderPass  GetRenderPass();
		VkFramebuffer GetFramebuffer();

	  private:
		void CreateFramebuffer();

		void AttachColourTargets();
		void AttachDepthTargets();
		void AttachResolveTargets();

		void CreateRenderPass();
		void CreateVulkanFramebuffer();

	  private:
		GraphicsDeviceVk				*m_Device;
		FramebufferTextureSetDescription m_Description = {};

		std::vector<const TextureVk *> m_ColourAttachments	= {};
		const TextureVk				  *m_DepthAttachment	= nullptr;
		std::vector<const TextureVk *> m_ResolveAttachments = {};

		VkRenderPass  m_RenderPass	= VK_NULL_HANDLE;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};
}	 // namespace Nexus::Graphics

#endif