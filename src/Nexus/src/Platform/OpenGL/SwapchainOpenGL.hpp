#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

	#include "Nexus-Core/Graphics/Swapchain.hpp"
	#include "Nexus-Core/IWindow.hpp"

	#include "Context/IOffscreenContext.hpp"
	#include "Context/IViewContext.hpp"

namespace Nexus::Graphics
{
	class SwapchainOpenGL : public Swapchain
	{
	  public:
		SwapchainOpenGL(IWindow *window, const SwapchainDescription &swapchainSpec, GraphicsDevice *graphicsDevice);
		virtual ~SwapchainOpenGL();
		void					 SwapBuffers() final;
		void					 SetPresentMode(PresentMode presentMode) final;
		Nexus::Point2D<uint32_t> GetSize() final;
		PixelFormat				 GetColourFormat() final;
		PixelFormat				 GetDepthFormat() final;

		void ResizeIfNecessary();
		void BindAsDrawTarget();

		GL::IViewContext *GetViewContext();

		IWindow *GetWindow() final
		{
			return m_Window;
		}

	  private:
		IWindow *m_Window;

		uint32_t						  m_SwapchainWidth	= 0;
		uint32_t						  m_SwapchainHeight = 0;
		std::unique_ptr<GL::IViewContext> m_ViewContext		= {};
	};
}	 // namespace Nexus::Graphics

#endif