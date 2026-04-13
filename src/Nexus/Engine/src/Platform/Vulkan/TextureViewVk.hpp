#pragma once

#include "GraphicsDeviceVk.hpp"
#include "RHI/TextureView.hpp"

namespace Nexus::Graphics
{
	class TextureViewVk : public ITextureView
	{
	  public:
		TextureViewVk(const TextureViewDescription &desc, GraphicsDeviceVk *device);
		virtual ~TextureViewVk();
		const TextureViewDescription &GetDescription() const final;
		const VkImageView			  GetVkImageView() const;

	  private:
		TextureViewDescription m_Description = {};
		GraphicsDeviceVk	  *m_Device		 = nullptr;
		VkImageView			   m_ImageView	 = VK_NULL_HANDLE;
	};
}	 // namespace Nexus::Graphics