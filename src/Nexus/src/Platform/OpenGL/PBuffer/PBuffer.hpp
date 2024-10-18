#pragma once

namespace Nexus::GL
{
	class PBuffer
	{
	  public:
		virtual ~PBuffer()
		{
		}
		virtual void MakeCurrent() = 0;
	};
}	 // namespace Nexus::GL