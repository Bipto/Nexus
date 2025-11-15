#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

	#include "Nexus-Core/Graphics/Swapchain.hpp"
	#include "Nexus-Core/IWindow.hpp"

	#include "Context/IOffscreenContext.hpp"
	#include "Context/IViewContext.hpp"

namespace Nexus::Graphics
{
	class GraphicsDeviceOpenGL;
	class FramebufferOpenGL;

	class SwapchainOpenGL : public ISwapchain
	{
	  public:
		SwapchainOpenGL(IWindow *window, const SwapchainDescription &swapchainSpec, GraphicsDeviceOpenGL *graphicsDevice);
		virtual ~SwapchainOpenGL();
		void					 SwapBuffers(const SwapchainPresentDescription &presentDesc) final;
		Ref<IFramebuffer>		 GetCurrentFramebuffer() final;
		void					 SetPresentMode(PresentMode presentMode) final;
		Nexus::Point2D<uint32_t> GetSize() final;
		PixelFormat				 GetColourFormat() final;
		PixelFormat				 GetDepthFormat() final;

		void ResizeIfNecessary();
		void BindAsDrawTarget();

		GL::IViewContext *GetViewContext();
		IWindow			 *GetWindow() final;
		Ref<IFramebuffer> GetFramebuffer();
		void			  CreateFramebuffer();

	  private:
		IWindow				 *m_Window = nullptr;
		GraphicsDeviceOpenGL *m_Device = nullptr;

		Ref<FramebufferOpenGL> m_Framebuffer = nullptr;

		uint32_t						  m_SwapchainWidth	= 0;
		uint32_t						  m_SwapchainHeight = 0;
		std::unique_ptr<GL::IViewContext> m_ViewContext		= {};

		PixelFormat m_ColourFormat = PixelFormat::R8_G8_B8_A8_UNorm;
		PixelFormat m_DepthFormat  = PixelFormat::D24_UNorm_S8_UInt;
	};
}	 // namespace Nexus::Graphics

#endif