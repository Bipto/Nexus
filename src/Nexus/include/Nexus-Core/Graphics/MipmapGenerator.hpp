#pragma once

#include "Nexus-Core/Graphics/FullscreenQuad.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class MipmapGenerator
	{
	  public:
		MipmapGenerator() = default;
		explicit MipmapGenerator(IGraphicsDevice *device, Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue);
		std::vector<char> GenerateMip(Ref<ITexture> texture, uint32_t levelToGenerate, uint32_t levelToGenerateFrom, uint32_t arrayLayer);

		static uint32_t GetMaximumNumberOfMips(uint32_t width, uint32_t height);

	  private:
		IGraphicsDevice	*m_Device	   = nullptr;
		Ref<ICommandList> m_CommandList = nullptr;
		FullscreenQuad	 m_Quad {};

		Ref<IShaderModule> m_VertexModule   = nullptr;
		Ref<IShaderModule> m_FragmentModule = nullptr;

		Ref<IGraphicsPipeline> m_Pipeline	= nullptr;
		Ref<IResourceSet>	  m_ResourceSet = nullptr;

		Nexus::Ref<Nexus::Graphics::ICommandQueue> m_CommandQueue = nullptr;
	};
}	 // namespace Nexus::Graphics