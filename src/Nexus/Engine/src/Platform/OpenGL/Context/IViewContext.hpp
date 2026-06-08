#pragma once

#include "IGLContext.hpp"
#include "Platform/OpenGL/ContextDescription.hpp"

#include "RHI/Structures.hpp"
#include "RHI/Texture.hpp"

namespace Nexus::Graphics
{
	// forward declaration
	class FramebufferOpenGL;
}	 // namespace Nexus::Graphics

namespace Nexus::GL
{

	class IViewContext : public IGLContext
	{
	  public:
		virtual ~IViewContext()
		{
		}
		virtual void					  Swap(Graphics::TextureHandle texture, const Graphics::SwapchainPresentDescription &presentDesc) = 0;
		virtual void					  SetVSync(bool enabled)																		  = 0;
		virtual const ContextDescription &GetDescription() const																		  = 0;

		virtual bool				 MakeCurrent()		= 0;
		virtual bool				 Validate()			= 0;
		virtual const GladGLContext &GetContext() const = 0;
	};
}	 // namespace Nexus::GL