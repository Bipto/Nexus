#pragma once

#if defined(NX_PLATFORM_OPENGL)

	#include "Nexus-Core/Graphics/Swapchain.hpp"
	#include "Nexus-Core/Window.hpp"

	#include "PBuffer/PBuffer.hpp"
	#include "FBO/FBO.hpp"

namespace Nexus::Graphics
{
	class SwapchainOpenGL : public Swapchain
	{
	  public:
		SwapchainOpenGL(Window *window, const SwapchainSpecification &swapchainSpec, GraphicsDevice *graphicsDevice);
		virtual ~SwapchainOpenGL();
		virtual void Initialise() override
		{
		}
		virtual void					 SwapBuffers() override;
		virtual VSyncState				 GetVsyncState() override;
		virtual void					 SetVSyncState(VSyncState vsyncState) override;
		virtual Nexus::Point2D<uint32_t> GetSize() override;
		void							 ResizeIfNecessary();
		void							 BindAsRenderTarget();
		void							 BindAsDrawTarget();

		virtual Window *GetWindow() override
		{
			return m_Window;
		}
		virtual void Prepare() override;

	  private:
		Window	  *m_Window;
		VSyncState m_VsyncState;

		uint32_t m_SwapchainWidth  = 0;
		uint32_t m_SwapchainHeight = 0;

		int			  m_Backbuffer = 0;

		std::unique_ptr<GL::FBO> m_FBO = {};
	};
}	 // namespace Nexus::Graphics

#endif