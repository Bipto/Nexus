#pragma once

namespace Nexus::GL
{
	class PBuffer
	{
	  public:
		virtual ~PBuffer()
		{
		}
		virtual bool MakeCurrent() = 0;
	};
}	 // namespace Nexus::GL