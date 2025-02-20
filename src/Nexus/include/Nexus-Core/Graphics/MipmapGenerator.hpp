#pragma once

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	struct MipData
	{
	  public:
		MipData() = delete;
		MipData(const std::vector<unsigned char> &pixels, uint32_t width, uint32_t height);

		uint32_t	GetWidth() const;
		uint32_t	GetHeight() const;
		const void *GetData() const;

	  private:
		std::vector<unsigned char> m_Pixels;
		uint32_t			   m_Width	= 0;
		uint32_t			   m_Height = 0;
	};

	class MipmapGenerator
	{
	  public:
		MipmapGenerator() = default;
		explicit MipmapGenerator(GraphicsDevice *device);
		std::vector<unsigned char> GenerateMip(Ref<Texture2D> texture, uint32_t levelToGenerate, uint32_t levelToGenerateFrom);

		static uint32_t GetMaximumNumberOfMips(uint32_t width, uint32_t height);

	  private:
		GraphicsDevice	*m_Device	   = nullptr;
		Ref<CommandList> m_CommandList = nullptr;
		FullscreenQuad	 m_Quad {};

		Ref<ShaderModule> m_VertexModule   = nullptr;
		Ref<ShaderModule> m_FragmentModule = nullptr;

		Ref<Pipeline>	 m_Pipeline	   = nullptr;
		Ref<ResourceSet> m_ResourceSet = nullptr;
	};
}	 // namespace Nexus::Graphics