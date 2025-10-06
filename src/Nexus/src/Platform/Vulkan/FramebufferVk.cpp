#if defined(NX_PLATFORM_VULKAN)

	#include "FramebufferVk.hpp"
	#include "Nexus-Core/Logging/Log.hpp"

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

	Ref<TextureVk> FramebufferVk::GetVulkanColourTexture(uint32_t index)
	{
		return m_ColourAttachments.at(index);
	}

	Ref<TextureVk> FramebufferVk::GetVulkanDepthTexture()
	{
		return m_DepthAttachment;
	}

	VkRenderPass FramebufferVk::GetRenderPass()
	{
		return m_RenderPass;
	}

	VkFramebuffer FramebufferVk::GetFramebuffer()
	{
		return m_Framebuffer;
	}

	void FramebufferVk::CreateFramebuffer()
	{
		const GladVulkanContext &context = m_Device->GetVulkanContext();

		AttachColourTargets();
		AttachDepthTargets();
		CreateVulkanFramebuffer();
	}

	void FramebufferVk::AttachColourTargets()
	{
		m_ColourAttachments.clear();

		for (int i = 0; i < m_Description.ColourAttachments.size(); i++)
		{
			const auto &colourAttachment = m_Description.ColourAttachments.at(i);
			m_ColourAttachments.push_back(std::dynamic_pointer_cast<TextureVk>(colourAttachment.TargetTexture));
		}
	}

	void FramebufferVk::AttachDepthTargets()
	{
		// check if there is a valid depth texture in the description and bind it to the framebuffer if there is
		if (m_Description.DepthAttachment.has_value())
		{
			Ref<Texture> texture = m_Description.DepthAttachment.value().TargetTexture;
			m_DepthAttachment	 = std::dynamic_pointer_cast<TextureVk>(texture);
		}
	}

	void FramebufferVk::CreateRenderPass()
	{
		Vk::VulkanRenderPassDescription renderPassDesc = {};

		for (const auto &colourAttachment : m_Description.ColourAttachments)
		{
			Ref<Texture> texture = colourAttachment.TargetTexture;
			renderPassDesc.ColourAttachments.push_back(Vk::GetVkPixelDataFormat(texture->GetPixelFormat()));
		}

		if (m_Description.DepthAttachment.has_value())
		{
			Ref<Texture> texture	   = m_Description.DepthAttachment.value().TargetTexture;
			renderPassDesc.DepthFormat = Vk::GetVkPixelDataFormat(texture->GetPixelFormat());
		}

		renderPassDesc.ResolveFormat = {};
		renderPassDesc.Samples		 = Vk::GetVkSampleCountFlagsFromSampleCount(m_Description.GetSampleCount());

		m_RenderPass = Vk::CreateRenderPass(m_Device, renderPassDesc);
	}

	void FramebufferVk::CreateVulkanFramebuffer()
	{
		Nexus::Point2D<uint32_t>		 size			 = m_Description.GetSize();
		Vk::VulkanFramebufferDescription framebufferDesc = {};

		for (Ref<TextureVk> colourAttachment : m_ColourAttachments) { framebufferDesc.ColourImageViews.push_back(colourAttachment->GetImageView()); }

		if (m_Description.DepthAttachment.has_value())
		{
			framebufferDesc.DepthImageView = m_DepthAttachment->GetImageView();
		}

		framebufferDesc.Width			 = size.X;
		framebufferDesc.Height			 = size.Y;
		framebufferDesc.VulkanRenderPass = m_RenderPass;

		m_Framebuffer = Vk::CreateFramebuffer(m_Device->GetVulkanContext(), m_Device->GetVkDevice(), framebufferDesc);
	}
}	 // namespace Nexus::Graphics

#endif