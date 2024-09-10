#pragma once

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	struct MipData
	{
	  public:
		MipData() = delete;
		MipData(const std::vector<std::byte> &pixels, uint32_t width, uint32_t height);

		uint32_t	GetWidth() const;
		uint32_t	GetHeight() const;
		const void *GetData() const;

	  private:
		std::vector<std::byte> m_Pixels;
		uint32_t			   m_Width	= 0;
		uint32_t			   m_Height = 0;
	};

	class MipmapGenerator
	{
	  public:
		MipmapGenerator() = default;
		explicit MipmapGenerator(GraphicsDevice *device);
		void GenerateMips(Ref<Texture2D> texture, uint32_t mipCount);

		static uint32_t GetMaximumNumberOfMips(uint32_t width, uint32_t height);

	  private:
		GraphicsDevice	*m_Device	   = nullptr;
		Ref<CommandList> m_CommandList = nullptr;
		FullscreenQuad	 m_Quad {};
	};
}	 // namespace Nexus::Graphics