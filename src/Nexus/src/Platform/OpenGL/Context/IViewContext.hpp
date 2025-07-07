#pragma once

#include "IGLContext.hpp"
#include "Platform/OpenGL/ContextSpecification.hpp"

namespace Nexus::GL
{
	class IViewContext : public IGLContext
	{
	  public:
		virtual ~IViewContext()
		{
		}
		virtual void						Swap()					 = 0;
		virtual void						SetVSync(bool enabled)	 = 0;
		virtual const ContextSpecification &GetSpecification() const = 0;
	};
}	 // namespace Nexus::GL