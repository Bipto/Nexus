#pragma once

namespace Nexus::GL
{
	class FBO
	{
	  public:
		virtual ~FBO()
		{
		}
		virtual void MakeCurrent()			= 0;
		virtual void Swap()					= 0;
		virtual void SetVSync(bool enabled) = 0;
	};
}	 // namespace Nexus::GL