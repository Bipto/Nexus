#pragma once

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Image.hpp"

namespace Nexus::Graphics
{
	class MipmapGenerator
	{
	  public:
		MipmapGenerator() = default;
		explicit MipmapGenerator(GraphicsDevice *device);
		std::vector<Image> GenerateMipChain(Image baseImage);
		Image			   GenerateMip(Ref<Texture2D> texture, uint32_t levelToGenerate, uint32_t levelToGenerateFrom);
		Image			   GenerateMipSoftware(const Image &baseImage, uint32_t levelToGenerate);
		static uint32_t	   GetMaximumNumberOfMips(uint32_t width, uint32_t height);

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