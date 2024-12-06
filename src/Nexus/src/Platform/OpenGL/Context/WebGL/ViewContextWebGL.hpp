#pragma once

#if defined(NX_PLATFORM_WEBGL)

	#include "Platform/OpenGL/Context/IViewContext.hpp"

	#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::GL
{
	struct BoundingClientRect
	{
		double Left	  = 0;
		double Top	  = 0;
		double Width  = 0;
		double Height = 0;
	};
	class ViewContextWebGL : public IViewContext
	{
	  public:
		ViewContextWebGL(const std::string &canvasName, Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice, const ContextSpecification &spec);
		virtual ~ViewContextWebGL();
		virtual bool						MakeCurrent() override;
		virtual void						Swap() override;
		virtual void						SetVSync(bool enabled) override;
		virtual const ContextSpecification &GetSpecification() const override;
		void								HandleResize();

	  private:
		void CreateFramebuffer();

	  private:
		Nexus::Graphics::GraphicsDeviceOpenGL *m_Device		   = nullptr;
		ContextSpecification				   m_Specification = {};
		std::string							   m_CanvasName	   = {};

		Ref<Graphics::Framebuffer> m_Framebuffer			= nullptr;
		BoundingClientRect		   m_BoundingClientRect		= {};
		inline static bool		   s_WindowResizeRegistered = false;
	};
};	  // namespace Nexus::GL

#endif