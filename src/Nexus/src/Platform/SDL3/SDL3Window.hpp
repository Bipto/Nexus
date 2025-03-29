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

		//==========================================================================================
		//									SET CALLBACKS
		//==========================================================================================
		void AddResizeCallback(std::function<void(const WindowResizedEventArgs &)> func) final;
		void AddMoveCallback(std::function<void(const WindowMovedEventArgs &)> func) final;

		void AddFocusGainCallback(std::function<void()> func) final;
		void AddFocusLostCallback(std::function<void()> func) final;
		void AddMaximizedCallback(std::function<void()> func) final;
		void AddMinimizedCallback(std::function<void()> func) final;
		void AddRestoreCallback(std::function<void()> func) final;
		void AddShowCallback(std::function<void()> func) final;
		void AddHideCallback(std::function<void()> func) final;
		void AddExposeCallback(std::function<void()> func) final;

		void AddKeyPressedCallback(std::function<void(const KeyPressedEventArgs &)> func) final;
		void AddKeyReleasedCallback(std::function<void(const KeyReleasedEventArgs &)> func) final;

		void AddTextInputCallback(std::function<void(const TextInputEventArgs &)> func) final;
		void AddTextEditCallback(std::function<void(const TextEditEventArgs &)> func) final;

		void AddMousePressedCallback(std::function<void(const MouseButtonPressedEventArgs &)> func) final;
		void AddMouseReleasedCallback(std::function<void(const MouseButtonReleasedEventArgs &)> func) final;
		void AddMouseMovedCallback(std::function<void(const MouseMovedEventArgs &)> func) final;
		void AddMouseScrollCallback(std::function<void(const MouseScrolledEventArgs &)> func) final;
		void AddMouseEnterCallback(std::function<void()> func) final;
		void AddMouseLeaveCallback(std::function<void()> func) final;

		void AddFileDropCallback(std::function<void(const FileDropEventArgs &)> func) final;

		//==========================================================================================
		//									USE CALLBACKS
		//==========================================================================================
		void InvokeResizeCallback(const WindowResizedEventArgs &args);
		void InvokeMoveCallback(const WindowMovedEventArgs &args);

		void InvokeFocusGainCallback();
		void InvokeFocusLostCallback();
		void InvokeMaximizedCallback();
		void InvokeMinimizedCallback();
		void InvokeRestoreCallback();
		void InvokeShowCallback();
		void InvokeHideCallback();
		void InvokeExposeCallback();

		void InvokeKeyPressedCallback(const KeyPressedEventArgs &args);
		void InvokeKeyReleasedCallback(const KeyReleasedEventArgs &args);

		void InvokeTextInputCallback(const TextInputEventArgs &args);
		void InvokeTextEditCallback(const TextEditEventArgs &args);

		void InvokeMousePressedCallback(const MouseButtonPressedEventArgs &args);
		void InvokeMouseReleasedCallback(const MouseButtonReleasedEventArgs &args);
		void InvokeMouseMovedCallback(const MouseMovedEventArgs &args);
		void InvokeMouseScrollCallback(const MouseScrolledEventArgs &args);
		void InvokeMouseEnterCallback();
		void InvokeMouseLeaveCallback();

		void InvokeFileDropCallback(const FileDropEventArgs &args);

		bool		   IsKeyDown(uint32_t keyboardId, ScanCode scancode) final;
		bool		   IsKeyDown(ScanCode scancode) final;
		Point2D<float> GetMousePosition(uint32_t mouseId) final;
		Point2D<float> GetMouseScroll(uint32_t mouseId) final;
		Point2D<float> GetMousePosition() final;
		Point2D<float> GetMouseScroll() final;
		bool		   IsMouseButtonPressed(uint32_t mouseId, MouseButton state) final;
		bool		   IsMouseButtonPressed(MouseButton state) final;

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

		std::vector<std::function<void(const WindowResizedEventArgs &)>> m_OnResizeCallbacks;
		std::vector<std::function<void(const WindowMovedEventArgs &)>>	 m_OnMoveCallbacks;

		std::vector<std::function<void()>> m_OnFocusGainCallbacks;
		std::vector<std::function<void()>> m_OnFocusLostCallbacks;
		std::vector<std::function<void()>> m_OnMaximizeCallbacks;
		std::vector<std::function<void()>> m_OnMinimizeCallbacks;
		std::vector<std::function<void()>> m_OnRestoreCallbacks;
		std::vector<std::function<void()>> m_OnShowCallbacks;
		std::vector<std::function<void()>> m_OnHideCallbacks;
		std::vector<std::function<void()>> m_OnExposeCallbacks;

		std::vector<std::function<void(const KeyPressedEventArgs &)>>  m_OnKeyPressedCallbacks;
		std::vector<std::function<void(const KeyReleasedEventArgs &)>> m_OnKeyReleasedCallbacks;

		std::vector<std::function<void(const TextInputEventArgs &)>> m_OnTextInputCallbacks;
		std::vector<std::function<void(const TextEditEventArgs &)>>	 m_OnTextEditCallbacks;

		std::vector<std::function<void(const MouseButtonPressedEventArgs &)>>  m_OnMouseButtonPressedCallbacks;
		std::vector<std::function<void(const MouseButtonReleasedEventArgs &)>> m_OnMouseButtonReleasedCallbacks;
		std::vector<std::function<void(const MouseMovedEventArgs &)>>		   m_OnMouseMovedCallbacks;
		std::vector<std::function<void(const MouseScrolledEventArgs &)>>	   m_OnMouseScrolledCallbacks;
		std::vector<std::function<void()>>									   m_OnMouseEnterCallbacks;
		std::vector<std::function<void()>>									   m_OnMouseLeaveCallbacks;

		std::vector<std::function<void(const FileDropEventArgs &)>> m_OnFileDropCallbacks;

		std::map<uint32_t, KeyboardState> m_KeyboardStates;
		std::map<uint32_t, MouseState>	  m_MouseStates;
	};
}	 // namespace Nexus