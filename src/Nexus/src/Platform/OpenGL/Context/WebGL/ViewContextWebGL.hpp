#pragma once

#if defined(NX_PLATFORM_WEBGL)

	#include "Platform/OpenGL/Context/IViewContext.hpp"

	#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"

namespace Nexus::GL
{
	class ViewContextWebGL : public IViewContext
	{
	  public:
		ViewContextWebGL(const std::string &canvasName, Nexus::Graphics::GraphicsDeviceOpenGL *graphicsDevice, const ContextSpecification &spec);
		virtual ~ViewContextWebGL();
		virtual bool						MakeCurrent() override;
		virtual void						BindAsRenderTarget() override;
		virtual void						BindAsDrawTarget() override;
		virtual void						Swap() override;
		virtual void						SetVSync(bool enabled) override;
		virtual const ContextSpecification &GetSpecification() const override;

	  private:
		Nexus::Graphics::GraphicsDeviceOpenGL *m_Device		   = nullptr;
		ContextSpecification				   m_Specification = {};
		std::string							   m_CanvasName	   = {};

		Ref<Graphics::Framebuffer> m_Framebuffer = nullptr;
		std::vector<unsigned char> m_Pixels		 = {};
	};
};	  // namespace Nexus::GL

#endif