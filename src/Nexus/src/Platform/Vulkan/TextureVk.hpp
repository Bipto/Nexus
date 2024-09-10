#pragma once

#if defined(NX_PLATFORM_VULKAN)

	#include "GraphicsDeviceVk.hpp"
	#include "Nexus-Core/Graphics/Texture.hpp"
	#include "Vk.hpp"

namespace Nexus::Graphics
{
	class Texture2D_Vk : public Texture2D
	{
	  public:
		Texture2D_Vk(GraphicsDeviceVk *graphicsDevice, const Texture2DSpecification &spec);
		~Texture2D_Vk();
		virtual void				   SetData(const void *data, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual std::vector<std::byte> GetData(uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		VkImage						   GetImage();
		VkImageView					   GetImageView();

		VkImageLayout GetImageLayout(uint32_t level);
		void		  SetImageLayout(uint32_t level, VkImageLayout layout);

	  private:
		GraphicsDeviceVk		  *m_GraphicsDevice;
		VkImage					   m_Image;
		VmaAllocation			   m_Allocation;
		VkImageView				   m_ImageView;
		VkFormat				   m_Format;
		Vk::AllocatedBuffer		   m_StagingBuffer;
		std::vector<VkImageLayout> m_Layouts;
	};

	class Cubemap_Vk : public Cubemap
	{
	  public:
		Cubemap_Vk(GraphicsDeviceVk *graphicsDevice, const CubemapSpecification &spec);
		~Cubemap_Vk();
		virtual void SetData(const void *data, CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual std::vector<std::byte> GetData(CubemapFace face, uint32_t level, uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		VkImage		GetImage();
		VkImageView GetImageView();

		VkImageLayout GetImageLayout(uint32_t arrayLayer, uint32_t mipLevel);
		void		  SetImageLayout(uint32_t arrayLayer, uint32_t mipLevel, VkImageLayout layout);

	  private:
		GraphicsDeviceVk					   *m_GraphicsDevice;
		VkImage									m_Image;
		VmaAllocation							m_Allocation;
		VkImageView								m_ImageView;
		VkFormat								m_Format;
		Vk::AllocatedBuffer						m_StagingBuffer;
		std::vector<std::vector<VkImageLayout>> m_Layouts;
	};
}	 // namespace Nexus::Graphics

#endif