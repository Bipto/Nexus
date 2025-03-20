#pragma once

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
namespace Nexus::Processors
{
	class NX_API TextureProcessor : public IProcessor
	{
	  public:
		TextureProcessor();
		virtual ~TextureProcessor();
		GUID Process(const std::string &filepath, Graphics::GraphicsDevice *device) final;
		void SetSrgb(bool useSrgb);
		void SetGenerateMips(bool generateMips);

	  private:
		bool m_GenerateMips = true;
		bool m_Srgb			= false;
	};
}	 // namespace Nexus::Processors
