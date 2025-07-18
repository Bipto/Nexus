#if defined(NX_PLATFORM_VULKAN)

	#include "FramebufferVk.hpp"

namespace Nexus::Graphics
{
	FramebufferVk::FramebufferVk(const FramebufferSpecification &spec, GraphicsDeviceVk *device) : Framebuffer(spec), m_Device(device)
	{
		m_Description = spec;
		CreateRenderPass();
		Recreate();
	}

	FramebufferVk::~FramebufferVk()
	{
		vkDestroyFramebuffer(m_Device->GetVkDevice(), m_Framebuffer, nullptr);
		vkDestroyRenderPass(m_Device->GetVkDevice(), m_RenderPass, nullptr);
	}

	const FramebufferSpecification FramebufferVk::GetFramebufferSpecification()
	{
		return m_Description;
	}

	void FramebufferVk::SetFramebufferSpecification(const FramebufferSpecification &spec)
	{
		m_Description = spec;
		Recreate();
	}

	Ref<Texture> FramebufferVk::GetColorTexture(uint32_t index)
	{
		return m_ColorAttachments.at(index);
	}

	Ref<Texture> FramebufferVk::GetDepthTexture()
	{
		return m_DepthAttachment;
	}

	Ref<TextureVk> FramebufferVk::GetVulkanColorTexture(uint32_t index)
	{
		return m_ColorAttachments.at(index);
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

	void FramebufferVk::Recreate()
	{
		vkDestroyFramebuffer(m_Device->GetVkDevice(), m_Framebuffer, nullptr);
		m_Framebuffer = VK_NULL_HANDLE;

		CreateColorTargets();
		CreateDepthTargets();
		CreateFramebuffer();
	}

	void FramebufferVk::CreateColorTargets()
	{
		m_ColorAttachments.clear();

		for (int i = 0; i < m_Description.ColourAttachmentSpecification.Attachments.size(); i++)
		{
			const auto &colorAttachmentSpec = m_Description.ColourAttachmentSpecification.Attachments.at(i);

			if (colorAttachmentSpec.TextureFormat == PixelFormat::Invalid)
			{
				NX_ASSERT(0, "Pixel format cannot be PixelFormat::None for a color attachment");
			}

			Graphics::TextureDescription spec	= {};
			spec.Width							= m_Description.Width;
			spec.Height							= m_Description.Height;
			spec.Format							= colorAttachmentSpec.TextureFormat;
			spec.Samples						= m_Description.Samples;
			spec.Usage							= Graphics::TextureUsage_Sampled | Graphics::TextureUsage_RenderTarget;

			Ref<Texture> texture = Ref<Texture>(m_Device->CreateTexture(spec));
			m_ColorAttachments.push_back(std::dynamic_pointer_cast<TextureVk>(texture));
		}
	}

	void FramebufferVk::CreateDepthTargets()
	{
		// the specification does not contain a depth attachment, so we do not create
		// one
		if (m_Description.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid)
		{
			Graphics::TextureDescription spec	= {};
			spec.Width							= m_Description.Width;
			spec.Height							= m_Description.Height;
			spec.Format							= m_Description.DepthAttachmentSpecification.DepthFormat;
			spec.Samples						= m_Description.Samples;
			spec.Usage							= 0;
			Ref<Texture> texture				= Ref<Texture>(m_Device->CreateTexture(spec));
			m_DepthAttachment					= std::dynamic_pointer_cast<TextureVk>(texture);
		}
	}

	void FramebufferVk::CreateRenderPass()
	{
		Vk::VulkanRenderPassDescription renderPassDesc = {};

		for (const auto &colourAttachment : m_Description.ColourAttachmentSpecification.Attachments)
		{
			renderPassDesc.ColourAttachments.push_back(Vk::GetVkPixelDataFormat(colourAttachment.TextureFormat));
		}

		if (m_Description.DepthAttachmentSpecification.DepthFormat != Nexus::Graphics::PixelFormat::Invalid)
		{
			renderPassDesc.DepthFormat = Vk::GetVkPixelDataFormat(m_Description.DepthAttachmentSpecification.DepthFormat);
		}

		renderPassDesc.ResolveFormat = {};
		renderPassDesc.Samples		 = Vk::GetVkSampleCountFlagsFromSampleCount(m_Description.Samples);

		m_RenderPass = Vk::CreateRenderPass(m_Device, renderPassDesc);
	}

	void FramebufferVk::CreateFramebuffer()
	{
		Vk::VulkanFramebufferDescription framebufferDesc = {};

		for (Ref<TextureVk> colourAttachment : m_ColorAttachments) { framebufferDesc.ColourImageViews.push_back(colourAttachment->GetImageView()); }

		if (m_Description.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid)
		{
			framebufferDesc.DepthImageView = m_DepthAttachment->GetImageView();
		}

		framebufferDesc.Width			 = m_Description.Width;
		framebufferDesc.Height			 = m_Description.Height;
		framebufferDesc.VulkanRenderPass = m_RenderPass;

		m_Framebuffer = Vk::CreateFramebuffer(m_Device->GetVkDevice(), framebufferDesc);
	}
}	 // namespace Nexus::Graphics

#endif