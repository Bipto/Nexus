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
		FramebufferVk(const FramebufferSpecification &spec, GraphicsDeviceVk *device);
		~FramebufferVk();

		virtual const FramebufferSpecification GetFramebufferSpecification() override;
		virtual void						   SetFramebufferSpecification(const FramebufferSpecification &spec) override;

		virtual Ref<Texture> GetColorTexture(uint32_t index = 0) override;
		virtual Ref<Texture> GetDepthTexture() override;

		Ref<TextureVk> GetVulkanColorTexture(uint32_t index = 0);
		Ref<TextureVk> GetVulkanDepthTexture();

	  private:
		virtual void Recreate() override;

		void CreateColorTargets();
		void CreateDepthTargets();

	  private:
		GraphicsDeviceVk *m_Device;

		std::vector<Ref<TextureVk>> m_ColorAttachments;
		Ref<TextureVk>				m_DepthAttachment = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif