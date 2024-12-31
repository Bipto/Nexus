#pragma once

#include "Nexus-Core/IResource.hpp"
#include "Platform/OpenGL/ContextSpecification.hpp"

namespace Nexus::GL
{
	class IViewContext
	{
	  public:
		virtual ~IViewContext()
		{
		}
		virtual bool						MakeCurrent()			 = 0;
		virtual void						Swap()					 = 0;
		virtual void						SetVSync(bool enabled)	 = 0;
		virtual const ContextSpecification &GetSpecification() const = 0;
		virtual bool						Validate()				 = 0;
	};
}	 // namespace Nexus::GL