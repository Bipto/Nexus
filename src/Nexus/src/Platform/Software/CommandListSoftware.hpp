#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/CommandList.hpp"

namespace Nexus::Graphics
{
	class CommandListSoftware : public CommandList
	{
	  public:
		CommandListSoftware(const CommandListSpecification &spec) : CommandList(spec)
		{
		}

		virtual ~CommandListSoftware()
		{
		}
	};
}	 // namespace Nexus::Graphics