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
		unsigned int CreateShader();
		unsigned int CreateVBO();
		unsigned int CreateVAO();
		unsigned int CreateShaderStage(const std::string &shader, GLenum stage);

	  private:
		Nexus::Graphics::GraphicsDeviceOpenGL *m_Device		   = nullptr;
		ContextSpecification				   m_Specification = {};
		std::string							   m_CanvasName	   = {};

		Ref<Graphics::Framebuffer> m_Framebuffer = nullptr;
		std::vector<unsigned char> m_Pixels		 = {};

		unsigned int m_ShaderHandle = 0;
		unsigned int m_VBO			= 0;
		unsigned int m_VAO			= 0;
	};
};	  // namespace Nexus::GL

#endif