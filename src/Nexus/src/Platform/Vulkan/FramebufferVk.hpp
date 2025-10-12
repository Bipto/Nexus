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
		FramebufferVk(const FramebufferTextureSetDescription	  &desc,
					  GraphicsDeviceVk							  *device,
					  std::optional<FramebufferTextureDescription> resolveAttachment);
		~FramebufferVk();

		const FramebufferTextureSetDescription GetTextureSetDescription() const final;

		Ref<TextureVk> GetVulkanColourTexture(uint32_t index = 0);
		Ref<TextureVk> GetVulkanDepthTexture();

		std::optional<FramebufferTextureDescription> GetResolveAttachmentBinding();
		bool										 HasResolveAttachment();

		VkRenderPass  GetRenderPass();
		VkFramebuffer GetFramebuffer();

	  private:
		void CreateFramebuffer();

		void AttachColourTargets();
		void AttachDepthTargets();

		void CreateRenderPass();
		void CreateVulkanFramebuffer();

	  private:
		GraphicsDeviceVk				*m_Device;
		FramebufferTextureSetDescription m_Description = {};

		std::vector<Ref<TextureVk>>					 m_ColourAttachments;
		Ref<TextureVk>								 m_DepthAttachment	 = nullptr;
		std::optional<FramebufferTextureDescription> m_ResolveAttachment = {};

		VkRenderPass  m_RenderPass	= VK_NULL_HANDLE;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};
}	 // namespace Nexus::Graphics

#endif