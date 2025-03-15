#pragma once

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
namespace Nexus::Processors
{
	class TextureProcessor : public IProcessor
	{
	  public:
		TextureProcessor()
		{
		}

		virtual ~TextureProcessor()
		{
		}

		GUID Process(const std::string &filepath) final;
	};
}	 // namespace Nexus::Processors
