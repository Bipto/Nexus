#if defined(NX_PLATFORM_VULKAN)

	#include "FramebufferVk.hpp"

namespace Nexus::Graphics
{
	FramebufferVk::FramebufferVk(const FramebufferSpecification &spec, GraphicsDeviceVk *device) : Framebuffer(spec), m_Device(device)
	{
		m_Specification = spec;
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
		return m_Specification;
	}

	void FramebufferVk::SetFramebufferSpecification(const FramebufferSpecification &spec)
	{
		m_Specification = spec;
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

		for (int i = 0; i < m_Specification.ColorAttachmentSpecification.Attachments.size(); i++)
		{
			const auto &colorAttachmentSpec = m_Specification.ColorAttachmentSpecification.Attachments.at(i);

			if (colorAttachmentSpec.TextureFormat == PixelFormat::Invalid)
			{
				NX_ASSERT(0, "Pixel format cannot be PixelFormat::None for a color attachment");
			}

			Graphics::TextureSpecification spec = {};
			spec.Width							= m_Specification.Width;
			spec.Height							= m_Specification.Height;
			spec.Format							= colorAttachmentSpec.TextureFormat;
			spec.Samples						= m_Specification.Samples;
			spec.Usage							= Graphics::TextureUsage_Sampled | Graphics::TextureUsage_RenderTarget;

			Ref<Texture> texture = Ref<Texture>(m_Device->CreateTexture(spec));
			m_ColorAttachments.push_back(std::dynamic_pointer_cast<TextureVk>(texture));
		}
	}

	void FramebufferVk::CreateDepthTargets()
	{
		// the specification does not contain a depth attachment, so we do not create
		// one
		if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid)
		{
			Graphics::TextureSpecification spec = {};
			spec.Width							= m_Specification.Width;
			spec.Height							= m_Specification.Height;
			spec.Format							= m_Specification.DepthAttachmentSpecification.DepthFormat;
			spec.Samples						= m_Specification.Samples;
			spec.Usage							= Graphics::TextureUsage_DepthStencil;
			Ref<Texture> texture				= Ref<Texture>(m_Device->CreateTexture(spec));
			m_DepthAttachment					= std::dynamic_pointer_cast<TextureVk>(texture);
		}
	}

	void FramebufferVk::CreateRenderPass()
	{
		Vk::VulkanRenderPassDescription renderPassDesc = {};

		for (const auto &colourAttachment : m_Specification.ColorAttachmentSpecification.Attachments)
		{
			renderPassDesc.ColourAttachments.push_back(Vk::GetVkPixelDataFormat(colourAttachment.TextureFormat, false));
		}

		if (m_Specification.DepthAttachmentSpecification.DepthFormat != Nexus::Graphics::PixelFormat::Invalid)
		{
			renderPassDesc.DepthFormat = Vk::GetVkPixelDataFormat(m_Specification.DepthAttachmentSpecification.DepthFormat, true);
		}

		renderPassDesc.ResolveFormat = {};
		renderPassDesc.Samples		 = Vk::GetVkSampleCountFlagsFromSampleCount(m_Specification.Samples);

		m_RenderPass = Vk::CreateRenderPass(m_Device->GetVkDevice(), renderPassDesc);
	}

	void FramebufferVk::CreateFramebuffer()
	{
		Vk::VulkanFramebufferDescription framebufferDesc = {};

		for (Ref<TextureVk> colourAttachment : m_ColorAttachments) { framebufferDesc.ColourImageViews.push_back(colourAttachment->GetImageView()); }

		if (m_Specification.DepthAttachmentSpecification.DepthFormat != PixelFormat::Invalid)
		{
			framebufferDesc.DepthImageView = m_DepthAttachment->GetImageView();
		}

		framebufferDesc.Width			 = m_Specification.Width;
		framebufferDesc.Height			 = m_Specification.Height;
		framebufferDesc.VulkanRenderPass = m_RenderPass;

		m_Framebuffer = Vk::CreateFramebuffer(m_Device->GetVkDevice(), framebufferDesc);
	}
}	 // namespace Nexus::Graphics

#endif