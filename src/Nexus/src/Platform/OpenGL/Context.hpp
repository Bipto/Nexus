#pragma once

namespace Nexus::GL
{
	enum class ContextSource
	{
		PBuffer,
		Swapchain
	};

	class Context
	{
	  public:
		virtual ~Context()
		{
		}
		virtual void		  Swap()		= 0;
		virtual void		  MakeCurrent() = 0;
		virtual ContextSource GetSource()	= 0;
	};
}	 // namespace Nexus::GL