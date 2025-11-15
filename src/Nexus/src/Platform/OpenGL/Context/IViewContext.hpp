#pragma once

#include "IGLContext.hpp"
#include "Platform/OpenGL/ContextSpecification.hpp"

#include "Nexus-Core/Graphics/Structures.hpp"

namespace Nexus::Graphics
{
	// forward declaration
	class FramebufferOpenGL;
	class TextureOpenGL;
}	 // namespace Nexus::Graphics

namespace Nexus::GL
{

	class IViewContext : public IGLContext
	{
	  public:
		virtual ~IViewContext()
		{
		}
		virtual void						Swap(Ref<Graphics::TextureOpenGL> texture, const Graphics::SwapchainPresentDescription &presentDesc) = 0;
		virtual void						SetVSync(bool enabled)																				 = 0;
		virtual const ContextSpecification &GetDescription() const																				 = 0;
	};
}	 // namespace Nexus::GL