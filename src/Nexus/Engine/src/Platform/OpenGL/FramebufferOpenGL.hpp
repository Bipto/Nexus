#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "GL.hpp"
	#include "RHI/Framebuffer.hpp"

namespace Nexus::Graphics
{
	// forward declaration
	class GraphicsDeviceOpenGL;

	class FramebufferOpenGL : public IFramebuffer
	{
	  public:
		FramebufferOpenGL(const FramebufferTextureSetDescription &desc, GraphicsDeviceOpenGL *graphicsDevice);
		~FramebufferOpenGL();
		const FramebufferTextureSetDescription GetTextureSetDescription() const final;

		void	BindAsReadBuffer(uint32_t texture, const GladGLContext &context);
		void	BindAsDrawBuffer(const GladGLContext &context);
		void	Unbind();
		int32_t GetHandle();

	  private:
		void Create();

	  private:
		FramebufferTextureSetDescription m_Description = {};
		uint32_t						 m_FBO		   = 0;
		GraphicsDeviceOpenGL			*m_Device	   = nullptr;
	};
}	 // namespace Nexus::Graphics

#endif