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
		SwapchainOpenGL(IWindow *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice);
		virtual ~SwapchainOpenGL();
		virtual void Initialise() override
		{
		}
		virtual void					 SwapBuffers() override;
		virtual VSyncState				 GetVsyncState() override;
		virtual void					 SetVSyncState(VSyncState vsyncState) override;
		virtual Nexus::Point2D<uint32_t> GetSize() override;
		virtual PixelFormat				 GetColourFormat() override;

		void							 ResizeIfNecessary();
		void							 BindAsDrawTarget();

		virtual IWindow *GetWindow() override
		{
			return m_Window;
		}
		virtual void Prepare() override;

	  private:
		IWindow	  *m_Window;
		VSyncState m_VsyncState;

		uint32_t						  m_SwapchainWidth	= 0;
		uint32_t						  m_SwapchainHeight = 0;
		std::unique_ptr<GL::IViewContext> ViewContext		= {};
	};
}	 // namespace Nexus::Graphics

#endif