#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	class IResource
	{
		NX_API virtual const std::string &GetName() const;
		NX_API virtual void				  SetName(const std::string &name);
	};
}	 // namespace Nexus::Graphics