#pragma once

#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Processors
{
	class IProcessor
	{
	  public:
		IProcessor()									  = default;
		virtual ~IProcessor()							  = default;
		virtual GUID Process(const std::string &filepath) = 0;
	};
}	 // namespace Nexus::Processors