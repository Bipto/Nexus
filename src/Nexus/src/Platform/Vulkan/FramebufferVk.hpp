#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/Framebuffer.hpp"
	#include "TextureVk.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class FramebufferVk : public Framebuffer
	{
	  public:
		FramebufferVk(const FramebufferTextureSetDescription &desc, GraphicsDeviceVk *device);
		~FramebufferVk();

		const FramebufferTextureSetDescription GetTextureSetDescription() const final;

		Ref<TextureVk> GetVulkanColourTexture(uint32_t index = 0);
		Ref<TextureVk> GetVulkanDepthTexture();
		Ref<TextureVk> GetVulkanResolveTexture(uint32_t index = 0);

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

		std::vector<Ref<TextureVk>> m_ColourAttachments	 = {};
		Ref<TextureVk>				m_DepthAttachment	 = nullptr;
		std::vector<Ref<TextureVk>> m_ResolveAttachments = {};

		VkRenderPass  m_RenderPass	= VK_NULL_HANDLE;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};
}	 // namespace Nexus::Graphics

#endif