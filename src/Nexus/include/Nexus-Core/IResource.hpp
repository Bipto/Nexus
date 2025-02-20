#pragma once

#include "nxpch.hpp"

namespace Nexus
{
	class IResource
	{
	  public:
		virtual bool			   Validate()						= 0;
		virtual void			   SetName(const std::string &name) = 0;
		virtual const std::string &GetName()						= 0;
	};
}	 // namespace Nexus