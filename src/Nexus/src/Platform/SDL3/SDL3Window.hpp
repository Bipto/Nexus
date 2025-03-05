#pragma once

#include "Nexus-Core/IWindow.hpp"

#include "Platform/SDL3/SDL3Include.hpp"

namespace Nexus
{
	class SDL3Window : public IWindow
	{
	  public:
		SDL3Window(const WindowSpecification &windowProps);
		SDL3Window(const SDL3Window &) = delete;
		virtual ~SDL3Window();

		void CacheInput() final;

		void Update() final;

		/// @brief A method that allows a window to be resized
		/// @param isResizable Whether the window should be resizable
		void SetResizable(bool isResizable) final;

		/// @brief A method that sets the title of the window
		/// @param title A const reference to a string containing the new title
		void SetTitle(const std::string &title) final;

		/// @brief A method to set the size of a window
		/// @param size
		void SetSize(Point2D<uint32_t> size) final;

		/// @brief A method that closes a window
		void Close() final;

		/// @brief A method that checks whether a window is closing
		/// @return A boolean value indicating whether the window is closing
		bool IsClosing() final;

		/// @brief A method that returns the underlying SDL window handle
		/// @return  A pointer to an SDL window
		SDL_Window *GetSDLWindowHandle();

		/// @brief A method that returns the size of the window
		/// @return A Nexus::Point containing two integers representing the size of
		/// the window
		Point2D<uint32_t> GetWindowSize() final;

		/// @brief A method that returns the size of the window in pixels
		/// @return A Nexus::Point containing two integers representing the pixel size of the window
		Point2D<uint32_t> GetWindowSizeInPixels() final;

		/// @brief A method to get the position of the window
		/// @return A Nexus::Point containing two integers representing the position
		/// of the window
		Point2D<int> GetWindowPosition() final;

		/// @brief A method that returns the current state of the window
		/// @return A WindowState enum value representing the state of the window
		WindowState GetCurrentWindowState() final;

		/// @brief A method that toggles whether the mouse is visible within the
		/// window
		/// @param visible A boolean value indicating whether the mouse should be
		/// visible
		void SetIsMouseVisible(bool visible) final;

		/// @brief A method that returns the window's input state
		/// @return A pointer to the input state
		InputState *GetInput() final;

		Nexus::InputNew::InputContext *GetInputContext() final;

		/// @brief A method that checks whether a window is focussed
		/// @return A boolean value indicating whether the window is focussed
		bool IsFocussed() final;

		bool IsMinimized() final;

		bool IsMaximized() final;

		bool IsFullscreen() final;

		/// @brief A method that maximizes a window
		void Maximize() final;

		/// @brief A method that minimizes a window
		void Minimize() final;

		/// @brief A method that restores a window to it's previous size
		void Restore() final;

		/// @brief A method that toggles a window between fullscreen and windowed
		void ToggleFullscreen() final;

		/// @brief A method that sets a window to be fullscreen
		void SetFullscreen() final;

		/// @brief A method that sets a window to be windowed
		void UnsetFullscreen() final;

		void Show() final;

		void Hide() final;

		void SetWindowPosition(int32_t x, int32_t y) final;

		void Focus() final;

		uint32_t GetID() final;

		float GetDisplayScale() final;

		void SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect) final;
		void StartTextInput() final;
		void StopTextInput() final;

		void SetRendersPerSecond(std::optional<uint32_t> amount) final;
		void SetUpdatesPerSecond(std::optional<uint32_t> amount) final;
		void SetTicksPerSecond(std::optional<uint32_t> amount) final;
		void SetRenderFunction(std::function<void(Nexus::TimeSpan time)> func) final;
		void SetUpdateFunction(std::function<void(Nexus::TimeSpan time)> func) final;
		void SetTickFunction(std::function<void(Nexus::TimeSpan time)> func) final;

		void SetRelativeMouseMode(bool enabled) final;

		NativeWindowInfo GetNativeWindowInfo() final;

	  private:
		/// @brief A method that returns a set of flags to use when creating the
		/// window
		/// @param api The graphics API to create the swapchain
		/// @return An unsigned int representing the flags
		uint32_t GetFlags(const WindowSpecification &windowSpec);

		void SetupTimer();

		const WindowSpecification &GetSpecification() const;

	  private:
		WindowSpecification m_Specification = {};

		/// @brief A pointer to the underlying SDL window
		SDL_Window *m_Window;

		/// @brief A boolean representing whether the window should close
		bool m_Closing = false;

		/// @brief A pointer to the window's input state
		InputState m_Input;

		/// @brief An enum value representing the current state of the window
		WindowState m_CurrentWindowState = WindowState::Normal;

		/// @brief The underlying SDL window ID
		uint32_t m_WindowID = 0;

		Nexus::Timings::ExecutionTimer m_Timer = {};

		bool m_Minimized = false;

		Utils::FrameRateMonitor m_RenderFrameRateMonitor = {};
		Utils::FrameRateMonitor m_UpdateFrameRateMonitor = {};
		Utils::FrameRateMonitor m_TickFrameRateMonitor	 = {};

		std::function<void(Nexus::TimeSpan)> m_RenderFunc;
		std::function<void(Nexus::TimeSpan)> m_UpdateFunc;
		std::function<void(Nexus::TimeSpan)> m_TickFunc;

		InputNew::InputContext m_InputContext = {nullptr};
	};
}	 // namespace Nexus