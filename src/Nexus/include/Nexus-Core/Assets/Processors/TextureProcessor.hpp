#pragma once

#include "Nexus-Core/Assets/Processors/IProcessor.hpp"
namespace Nexus::Processors
{
	class NX_API TextureProcessor : public IProcessor
	{
	  public:
		TextureProcessor();
		virtual ~TextureProcessor();
		GUID Process(const std::string &filepath) final;
	};
}	 // namespace Nexus::Processors
