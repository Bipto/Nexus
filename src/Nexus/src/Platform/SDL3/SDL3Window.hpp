#pragma once

#include "Nexus-Core/Window.hpp"

#include "Platform/SDL3/SDL3Include.hpp"

namespace Nexus
{
	/* class SDL3Window : public IWindow
	{
	  public:
		SDL3Window(const WindowSpecification &windowProps, Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec);
		SDL3Window(const Window &) = delete;
		virtual ~SDL3Window();

		virtual void SetPosition(int32_t x, int32_t y) override;
		virtual void SetResizable(bool resizable) override;
		virtual void SetTitle(const std::string &title) override;
		virtual void SetSize(const Point2D<uint32_t> &size) override;
		virtual void SetMouseVisible(bool enabled) override;
		virtual void SetRelativeMouse(bool enabled) override;
		virtual void SetCursor(Cursor cursor) override;

		virtual void SetRendersPerSecond(uint32_t time) override;
		virtual void SetUpdatesPerSecond(uint32_t time) override;
		virtual void SetTicksPerSecond(uint32_t time) override;

		virtual InputState					 *GetInput() override;
		virtual const InputNew::InputContext &GetInputContext() const override;

		virtual bool IsFocussed() const override;
		virtual bool IsFullscreen() const override;

		virtual float			  GetDPI() const override;
		virtual Point2D<uint32_t> GetSize() const override;
		virtual Point2D<int>	  GetPosition() const override;
		virtual WindowState		  GetState() const override;

		virtual void		 Close() override;
		virtual bool		 IsClosing() const override;
		virtual WindowHandle GetHandle() const override;

		virtual void Maximise() override;
		virtual void Minimise() override;
		virtual void Restore() override;
		virtual void SetFullscreen(bool enabled) override;
		virtual void Show() override;
		virtual void Hide() override;
		virtual void Focus() override;

		virtual void CreateSwapchain(Graphics::GraphicsDevice *device, const Graphics::SwapchainSpecification &swapchainSpec) override;
		virtual Graphics::Swapchain *GetSwapchain() override;

		virtual const WindowSpecification &GetSpecification() const override;

	  private:
		uint32_t GetFlags(Graphics::GraphicsAPI api, const WindowSpecification &windowSpec, const Graphics::SwapchainSpecification &swapchainSpec);
		void	 SetupTimer();

	  private:
		SDL_Window			  *m_Window		  = nullptr;
		bool				   m_Closing	  = false;
		InputState			   m_InputState	  = {};
		InputNew::InputContext m_InputContext = {};
		Graphics::Swapchain	  *m_Swapchain	  = nullptr;
		uint32_t			   m_WindowID	  = 0;

		Nexus::Timings::ExecutionTimer m_ExecutionTimer			= {};
		Utils::FrameRateMonitor		   m_RenderFrameRateMonitor = {};
		Utils::FrameRateMonitor		   m_UpdateFrameRateMonitor = {};
		Utils::FrameRateMonitor		   m_TickFrameRateMonitor	= {};

		WindowSpecification m_Specification = {};
	}; */
}	 // namespace Nexus