#pragma once

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
namespace Nexus::Processors
{
	class NX_API TextureProcessor : public IProcessor
	{
	  public:
		TextureProcessor();
		virtual ~TextureProcessor();
		GUID Process(const std::string &filepath, Graphics::GraphicsDevice *device, Project *project) final;
		void SetSrgb(bool useSrgb);
		void SetGenerateMips(bool generateMips);

	  private:
		bool m_GenerateMips = true;
		bool m_Srgb			= false;
	};
	// NX_REGISTER_PROCESSOR(TextureProcessor, "Texture2D", (std::vector<std::string> {".jpg", ".png", ".dds"}));
}	 // namespace Nexus::Processors

