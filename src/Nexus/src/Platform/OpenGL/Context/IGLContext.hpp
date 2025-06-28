#pragma once

namespace Nexus::GL
{
	class IGLContext
	{
	  public:
		virtual ~IGLContext()
		{
		}

		virtual bool MakeCurrent() = 0;
		virtual bool Validate()	   = 0;
	};
}	 // namespace Nexus::GL