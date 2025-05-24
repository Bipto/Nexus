#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
	enum class FenceWaitResult
	{
		TimedOut,
		Signalled,
		Failed
	};

	struct FenceDescription
	{
		bool Signalled = false;
	};

	class Fence
	{
	  public:
		virtual ~Fence()
		{
		}
		virtual bool					IsSignalled() const	   = 0;
		virtual const FenceDescription &GetDescription() const = 0;
	};
}	 // namespace Nexus::Graphics