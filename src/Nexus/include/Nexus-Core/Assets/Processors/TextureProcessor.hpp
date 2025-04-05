#pragma once

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"

#define TEXTURE_2D_PROCESSOR_NAME "Texture2D"

namespace Nexus::Processors
{
	class NX_API TextureProcessor : public IProcessor
	{
	  public:
		TextureProcessor() : IProcessor(TEXTURE_2D_PROCESSOR_NAME)
		{
		}
		virtual ~TextureProcessor() = default;
		GUID Process(const std::string &filepath, Graphics::GraphicsDevice *device, Project *project) final;
		virtual std::any Import(const std::string &filepath) final;
		void SetSrgb(bool useSrgb);
		void SetGenerateMips(bool generateMips);

	  private:
		bool m_GenerateMips = true;
		bool m_Srgb			= false;
	};
	NX_REGISTER_PROCESSOR(TextureProcessor, TEXTURE_2D_PROCESSOR_NAME, (std::vector<std::string> {".jpg", ".png", ".dds"}));
}	 // namespace Nexus::Processors

