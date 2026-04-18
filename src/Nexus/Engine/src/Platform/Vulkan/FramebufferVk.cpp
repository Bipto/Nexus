#if defined(NX_PLATFORM_VULKAN)

	#include "FramebufferVk.hpp"
	#include "Platform/Logging/Log.hpp"

namespace Nexus::Graphics
{
	FramebufferVk::FramebufferVk(const FramebufferTextureSetDescription &desc, GraphicsDeviceVk *device) : m_Device(device), m_Description(desc)
	{
		NX_VALIDATE(desc.ValidateSamples(), "Sample count must match across all textures in a framebuffer");
		NX_VALIDATE(desc.ValidateDimensions(), "The dimensions of all textures in a framebuffer must match");
		NX_VALIDATE(desc.ValidateUsageFlags(), "The usage flags of all textures must be correct for usage in a framebuffer");

		CreateRenderPass();
		CreateFramebuffer();
	}

	FramebufferVk::~FramebufferVk()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();
		context.DestroyFramebuffer(m_Device->GetVkDevice(), m_Framebuffer, nullptr);
		context.DestroyRenderPass(m_Device->GetVkDevice(), m_RenderPass, nullptr);
	}

	const FramebufferTextureSetDescription FramebufferVk::GetTextureSetDescription() const
	{
		return m_Description;
	}

	const TextureVk *FramebufferVk::GetVulkanColourTexture(uint32_t index) const
	{
		return m_ColourAttachments.at(index);
	}

	const TextureVk *FramebufferVk::GetVulkanDepthTexture() const
	{
		return m_DepthAttachment;
	}

	const TextureVk *FramebufferVk::GetVulkanResolveTexture(uint32_t index) const
	{
		return m_ResolveAttachments.at(index);
	}

	VkRenderPass FramebufferVk::GetRenderPass() const
	{
		return m_RenderPass;
	}

	VkFramebuffer FramebufferVk::GetFramebuffer() const
	{
		return m_Framebuffer;
	}

	void FramebufferVk::CreateFramebuffer()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		AttachColourTargets();
		AttachDepthTargets();
		AttachResolveTargets();
		CreateVulkanFramebuffer();
	}

	void FramebufferVk::AttachColourTargets()
	{
		m_ColourAttachments.clear();

		for (int i = 0; i < m_Description.ColourAttachments.size(); i++)
		{
			const auto &colourAttachment = m_Description.ColourAttachments.at(i);
			m_ColourAttachments.push_back(colourAttachment.ColourAttachment.TargetTexture.AsDerived<const TextureVk>());
		}
	}

	void FramebufferVk::AttachDepthTargets()
	{
		// check if there is a valid depth texture in the description and bind it to the framebuffer if there is
		if (m_Description.DepthAttachment.has_value())
		{
			TextureHandle texture = m_Description.DepthAttachment.value().TargetTexture;
			m_DepthAttachment	  = dynamic_cast<const TextureVk *>(texture.AsDerived<const TextureVk>());
		}
	}

	void FramebufferVk::AttachResolveTargets()
	{
		uint32_t samples = m_Description.GetSampleCount();
		if (samples > 1)
		{
			for (size_t i = 0; i < m_Description.ColourAttachments.size(); i++)
			{
				const auto &colourAttachmentDesc = m_Description.ColourAttachments.at(i);

				// if the colour attachment has a resolve attachment, we bind this
				if (colourAttachmentDesc.ResolveAttachment.has_value())
				{
					TextureHandle resolveTexture = colourAttachmentDesc.ResolveAttachment.value().TargetTexture;
					m_ResolveAttachments.push_back(resolveTexture.AsDerived<const TextureVk>());
				}
				// otherwise, we bind a placeholder to preserve alignment
				else
				{
					m_ResolveAttachments.push_back({});
				}
			}
		}
	}

	void FramebufferVk::CreateRenderPass()
	{
		Vk::VulkanRenderPassDescription renderPassDesc = {};

		for (const auto &colourAttachment : m_Description.ColourAttachments)
		{
			TextureHandle												 colourTexture = colourAttachment.ColourAttachment.TargetTexture;
			Vk::VulkanRenderPassDescription::VulkanColourAttachmentDesc &desc		   = renderPassDesc.ColourAttachments.emplace_back();
			desc.ColourFromat														   = Vk::GetVkPixelDataFormat(colourTexture->GetPixelFormat());

			if (colourAttachment.ResolveAttachment.has_value())
			{
				TextureHandle resolveTexture = colourAttachment.ResolveAttachment.value().TargetTexture;
				desc.ResolveFormat			 = Vk::GetVkPixelDataFormat(resolveTexture->GetPixelFormat());
			}
		}

		if (m_Description.DepthAttachment.has_value())
		{
			TextureHandle texture	   = m_Description.DepthAttachment.value().TargetTexture;
			renderPassDesc.DepthFormat = Vk::GetVkPixelDataFormat(texture->GetPixelFormat());
		}

		renderPassDesc.Samples = Vk::GetVkSampleCountFlagsFromSampleCount(m_Description.GetSampleCount());

		m_RenderPass = Vk::CreateRenderPass(m_Device, renderPassDesc);
	}

	void FramebufferVk::CreateVulkanFramebuffer()
	{
		Vk::VulkanFramebufferDescription framebufferDesc = {};

		for (size_t i = 0; i < m_Description.ColourAttachments.size(); i++)
		{
			const FramebufferColourAttachmentDescription &colourAttachmentDesc = m_Description.ColourAttachments.at(i);
			const TextureVk								 *colourTexture		   = m_ColourAttachments.at(i);

			Graphics::VulkanTextureViewInfo colourViewInfo = {};
			colourViewInfo.BaseArrayLayer				   = colourAttachmentDesc.ColourAttachment.BaseArrayLayer;
			colourViewInfo.LayerCount					   = colourAttachmentDesc.ColourAttachment.LayerCount;
			colourViewInfo.BaseMipLevel					   = colourAttachmentDesc.ColourAttachment.MipLevel;
			colourViewInfo.LevelCount					   = 1;

			Vk::VulkanFramebufferDescription::VulkanColourAttachment &framebufferColourAttachment = framebufferDesc.ColourImageViews.emplace_back();
			framebufferColourAttachment.ColourView												  = colourTexture->GetImageView(colourViewInfo);

			if (colourAttachmentDesc.ResolveAttachment.has_value())
			{
				const TextureVk				 *resolveTexture = m_ResolveAttachments.at(i);
				FramebufferTextureDescription resolveDesc	 = colourAttachmentDesc.ResolveAttachment.value();

				Graphics::VulkanTextureViewInfo resolveViewInfo = {};
				resolveViewInfo.BaseArrayLayer					= resolveDesc.BaseArrayLayer;
				resolveViewInfo.LayerCount						= resolveDesc.LayerCount;
				resolveViewInfo.BaseMipLevel					= resolveDesc.MipLevel;
				resolveViewInfo.LevelCount						= 1;

				framebufferColourAttachment.ResolveView = resolveTexture->GetImageView(resolveViewInfo);
			}
		}

		if (m_Description.DepthAttachment.has_value())
		{
			FramebufferTextureDescription depthAttachmentDesc = m_Description.DepthAttachment.value();

			Graphics::VulkanTextureViewInfo viewInfo = {};
			viewInfo.BaseArrayLayer					 = depthAttachmentDesc.BaseArrayLayer;
			viewInfo.LayerCount						 = depthAttachmentDesc.LayerCount;
			viewInfo.BaseMipLevel					 = depthAttachmentDesc.MipLevel;
			viewInfo.LevelCount						 = 1;
			framebufferDesc.DepthImageView			 = m_DepthAttachment->GetImageView(viewInfo);
		}

		auto [width, height]			 = m_Description.GetSize();
		framebufferDesc.Width			 = width;
		framebufferDesc.Height			 = height;
		framebufferDesc.VulkanRenderPass = m_RenderPass;

		m_Framebuffer = Vk::CreateFramebuffer(m_Device->GetVulkanContext(), m_Device->GetVkDevice(), framebufferDesc);
	}
}	 // namespace Nexus::Graphics

#endif