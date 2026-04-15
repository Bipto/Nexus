#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Context/IOffscreenContext.hpp"
	#include "Context/IViewContext.hpp"
	#include "RHI/GraphicsDevice.hpp"
	#include "RHI/Swapchain.hpp"
	#include "Surface/SurfaceOpenGL.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;
	class FramebufferOpenGL;

	class SwapchainOpenGL : public ISwapchain
	{
	  public:
		SwapchainOpenGL(const SwapchainDescription &swapchainSpec, GraphicsDeviceOpenGL *graphicsDevice);
		virtual ~SwapchainOpenGL();
		void							 SwapBuffers(const SwapchainPresentDescription &presentDesc) final;
		FramebufferHandle				 GetCurrentFramebuffer() final;
		void							 SetPresentMode(PresentMode presentMode) final;
		std::pair<uint32_t, uint32_t>	 GetSize() final;
		PixelFormat						 GetColourFormat() final;
		PixelFormat						 GetDepthFormat() final;
		std::expected<void, std::string> Resize(uint32_t width, uint32_t height) final;

		void BindAsDrawTarget();

		GL::IViewContext *GetViewContext();
		FramebufferHandle GetFramebuffer();
		void			  CreateFramebuffer();

	  private:
		GraphicsDeviceOpenGL *m_Device = nullptr;

		FramebufferHandle m_Framebuffer = {};

		uint32_t						  m_SwapchainWidth	= 0;
		uint32_t						  m_SwapchainHeight = 0;
		std::unique_ptr<GL::IViewContext> m_ViewContext		= {};

		PixelFormat m_ColourFormat = PixelFormat::R8_G8_B8_A8_UNorm;
		PixelFormat m_DepthFormat  = PixelFormat::D24_UNorm_S8_UInt;
	};
}	 // namespace Nexus::Graphics

#endif