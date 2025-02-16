#pragma once

#include "nxpch.hpp"

namespace Nexus
{
	class IResource
	{
	  public:
		NX_API virtual bool				  Validate()					   = 0;
		NX_API virtual void				  SetName(const std::string &name) = 0;
		NX_API virtual const std::string &GetName()						   = 0;
	};
}	 // namespace Nexus