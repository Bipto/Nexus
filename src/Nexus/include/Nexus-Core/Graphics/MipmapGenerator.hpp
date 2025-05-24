#pragma once

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class MipmapGenerator
	{
	  public:
		MipmapGenerator() = default;
		explicit MipmapGenerator(GraphicsDevice *device);
		std::vector<char> GenerateMip(Ref<Texture> texture, uint32_t levelToGenerate, uint32_t levelToGenerateFrom);

		static uint32_t GetMaximumNumberOfMips(uint32_t width, uint32_t height);

	  private:
		GraphicsDevice	*m_Device	   = nullptr;
		Ref<CommandList> m_CommandList = nullptr;
		FullscreenQuad	 m_Quad {};

		Ref<ShaderModule> m_VertexModule   = nullptr;
		Ref<ShaderModule> m_FragmentModule = nullptr;

		Ref<GraphicsPipeline> m_Pipeline	= nullptr;
		Ref<ResourceSet> m_ResourceSet = nullptr;
	};
}	 // namespace Nexus::Graphics