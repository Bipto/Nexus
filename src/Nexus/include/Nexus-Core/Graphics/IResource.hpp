#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class IResource
	{
		virtual const std::string &GetName() const;
		virtual void			   SetName(const std::string &name);
	};
}	 // namespace Nexus::Graphics