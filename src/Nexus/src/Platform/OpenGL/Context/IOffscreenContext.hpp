#pragma once

namespace Nexus::GL
{
	class IOffscreenContext
	{
	  public:
		virtual ~IOffscreenContext()
		{
		}
		virtual bool MakeCurrent() = 0;
		virtual bool Validate()	   = 0;
	};
}	 // namespace Nexus::GL