#pragma once

#include <utility>

#include "Platform/Events/EventHandler.hpp"
#include "Platform/Input/Events.hpp"
#include "Platform/Timings/Timer.hpp"
#include "Utils/FramerateMonitor.hpp"

#include "WindowInfo.hpp"

#include "Platform/Input/Input.hpp"

namespace Nexus
{
	/// @brief An enum representing the current window state
	enum class WindowState
	{
		/// @brief A value representing the window is in a default state
		Normal,

		/// @brief A value representing that the window has been minimized
		Minimized,

		/// @brief A value representing that the window has been maximized
		Maximized
	};

	enum WindowFlags : uint32_t
	{
		WindowFlags_None			  = 0,
		WindowFlags_Fullscreen		  = BIT(0),
		WindowFlags_Occluded		  = BIT(1),
		WindowFlags_Hidden			  = BIT(2),
		WindowFlags_Borderless		  = BIT(3),
		WindowFlags_Resizable		  = BIT(4),
		WindowFlags_Minimized		  = BIT(5),
		WindowFlags_Maximized		  = BIT(6),
		WindowFlags_MouseGrabbed	  = BIT(7),
		WindowFlags_InputFocus		  = BIT(8),
		WindowFlags_MouseFocus		  = BIT(9),
		WindowFlags_Modal			  = BIT(10),
		WindowFlags_HighPixelDensity  = BIT(11),
		WindowFlags_MouseCapture	  = BIT(12),
		WindowFlags_MouseRelativeMode = BIT(13),
		WindowFlags_AlwaysOnTop		  = BIT(14),
		WindowFlags_Utility			  = BIT(15),
		WindowFlags_Tooltip			  = BIT(16),
		WindowFlags_PopupMenu		  = BIT(17),
		WindowFlags_KeyboardGrabbed	  = BIT(18),
		WindowFlags_Transparent		  = BIT(19),
		WindowFlags_NotFocusable	  = BIT(20)
	};

	/// @brief A struct that represents a set of options for a window
	struct WindowDescription
	{
		/// @brief A string containing the title of the window
		std::string Title = "My Window";

		/// @brief An integer containing the width of the window
		uint32_t Width = 1280;

		/// @brief An integer containing the height of the window
		uint32_t Height = 720;

		uint32_t Flags = WindowFlags_None;

		std::optional<uint32_t> RendersPerSecond = {};
		std::optional<uint32_t> UpdatesPerSecond = {};
		std::optional<uint32_t> TicksPerSecond	 = {};

		std::string CanvasId = "canvas";
	};

	/// @brief A class representing a window
	class NX_PLATFORM_API IWindow
	{
	  public:
		/// @brief A constructor taking in a window properties struct
		/// @param windowProps A structure containing options controlling how the
		/// window is created
		IWindow(const WindowDescription &windowProps)
		{
		}

		/// @brief Copying a window is not supported
		/// @param A const reference to a window
		IWindow(const IWindow &) = delete;

		/// @brief A destructor to allow resources to be freed
		virtual ~IWindow()
		{
		}

		/// @brief A virtual method that performs all actions needed to process and update the window
		virtual void Update() = 0;

		/// @brief A method that allows a window to be resized
		/// @param isResizable Whether the window should be resizable
		virtual void SetResizable(bool isResizable) = 0;

		/// @brief A method that sets the title of the window
		/// @param title A const reference to a string containing the new title
		virtual void SetTitle(const std::string &title) = 0;

		/// @brief A method to set the size of a window
		/// @param size
		virtual void SetSize(uint32_t width, uint32_t height) = 0;

		/// @brief A method that closes a window
		virtual void Close() = 0;

		/// @brief A method that checks whether a window is closing
		/// @return A boolean value indicating whether the window is closing
		virtual bool IsClosing() = 0;

		/// @brief A method that returns the size of the window in relative units
		/// @return A Nexus::Point containing two integers representing the size of
		/// the window
		virtual std::pair<uint32_t, uint32_t> GetWindowSize() = 0;

		/// @brief A method that returns the size of the window in pixels
		/// @return A Nexus::Point containing two integers representing the pixel size of the window
		virtual std::pair<uint32_t, uint32_t> GetWindowSizeInPixels() = 0;

		/// @brief A method to get the position of the window
		/// @return A Nexus::Point containing two integers representing the position
		/// of the window
		virtual std::pair<int32_t, int32_t> GetWindowPosition() = 0;

		/// @brief A method that returns the current state of the window
		/// @return A WindowState enum value representing the state of the window
		virtual WindowState GetCurrentWindowState() = 0;

		/// @brief A method that toggles whether the mouse is visible within the
		/// window
		/// @param visible A boolean value indicating whether the mouse should be
		/// visible
		virtual void SetIsMouseVisible(bool visible) = 0;

		/// @brief A method that checks whether a window is focussed
		/// @return A boolean value indicating whether the window is focussed
		virtual bool IsFocussed() = 0;

		/// @brief A method that checks whether a window is minimized
		/// @return A boolean value indicating whether the window is minimized
		virtual bool IsMinimized() = 0;

		/// @brief A method that checks whether a window is maximized
		/// @return A boolean value indicating whether the window is maximized
		virtual bool IsMaximized() = 0;

		/// @brief A method that checks whether a window is currently fullscreen
		/// @return A boolean value indicating whether the window is fullscreen
		virtual bool IsFullscreen() = 0;

		/// @brief A method that maximizes a window
		virtual void Maximize() = 0;

		/// @brief A method that minimizes a window
		virtual void Minimize() = 0;

		/// @brief A method that restores a window to it's previous size
		virtual void Restore() = 0;

		/// @brief A method that toggles a window between fullscreen and windowed
		virtual void ToggleFullscreen() = 0;

		/// @brief A method that sets a window to be fullscreen
		virtual void SetFullscreen() = 0;

		/// @brief A method that sets a window to be windowed
		virtual void UnsetFullscreen() = 0;

		/// @brief A method that shows the window
		virtual void Show() = 0;

		/// @brief A method that hides the window
		virtual void Hide() = 0;

		/// @brief A method to focus a window
		virtual void Focus() = 0;

		/// @brief A method that sets the position of a window
		/// @param x The X coordinate to position the window to
		/// @param y The Y coordinate to position the window to
		virtual void SetWindowPosition(int32_t x, int32_t y) = 0;

		/// @brief A method that returns the ID of the window
		/// @return An unsigned 32 bit integer representing the ID of the window
		virtual uint32_t GetID() = 0;

		/// @brief A method that returns the display scale of the monitor that the window is on
		/// @return A float representing the display scale
		virtual float GetDisplayScale() = 0;

		/// @brief A virtual method that sets the text input rect
		/// @param rect The rectangle to set to accept input in
		virtual void SetTextInputRect(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

		/// @brief A virtual method that begins text input for a window
		virtual void StartTextInput() = 0;

		/// @brief A virtual method that ends text input for a window
		virtual void StopTextInput() = 0;

		/// @brief A virtual method that sets how often the window should be rendered to
		/// @param amount The amount of times per second to render, use null optional to disable timings
		virtual void SetRendersPerSecond(std::optional<uint32_t> amount) = 0;

		/// @brief A virtual method that sets how often the window should be updated
		/// @param amount The amount of times per second to update, use null optional to disable timings
		virtual void SetUpdatesPerSecond(std::optional<uint32_t> amount) = 0;

		/// @brief A virtual method that sets how often the window should be ticked
		/// @param amount The amount of times per second to ticked, use null optional to disable timings
		virtual void SetTicksPerSecond(std::optional<uint32_t> amount) = 0;

		/// @brief A virtual method that sets the function to call when the window is rendered to
		/// @param func The function to call when rendering
		virtual void SetRenderFunction(std::function<void(Nexus::TimeSpan time)> func) = 0;

		/// @brief A virtual method that sets the function to call when the window is updated
		/// @param func The function to call when updating
		virtual void SetUpdateFunction(std::function<void(Nexus::TimeSpan time)> func) = 0;

		/// @brief A virtual method that sets the function to call when the window is ticking
		/// @param func The function to call when ticking
		virtual void SetTickFunction(std::function<void(Nexus::TimeSpan time)> func) = 0;

		/// @brief A virtual method that sets whether the mouse is invisible and locked to the window
		/// @param enabled Whether the mouse is relative of not
		virtual void SetRelativeMouseMode(bool enabled) = 0;

		/// @brief A virtual method that returns a struct containing the native window data of the IWindow
		/// @return
		virtual NativeWindowInfo GetNativeWindowInfo() = 0;

		virtual void AddResizeCallback(std::function<void(const WindowResizedEventArgs &)> func) = 0;
		virtual void AddMoveCallback(std::function<void(const WindowMovedEventArgs &)> func)	 = 0;

		virtual void AddFocusGainCallback(std::function<void()> func) = 0;
		virtual void AddFocusLostCallback(std::function<void()> func) = 0;
		virtual void AddMaximizedCallback(std::function<void()> func) = 0;
		virtual void AddMinimizedCallback(std::function<void()> func) = 0;
		virtual void AddRestoreCallback(std::function<void()> func)	  = 0;
		virtual void AddShowCallback(std::function<void()> func)	  = 0;
		virtual void AddHideCallback(std::function<void()> func)	  = 0;
		virtual void AddExposeCallback(std::function<void()> func)	  = 0;

		virtual void AddKeyPressedCallback(std::function<void(const KeyPressedEventArgs &)> func)	= 0;
		virtual void AddKeyReleasedCallback(std::function<void(const KeyReleasedEventArgs &)> func) = 0;

		virtual void AddTextInputCallback(std::function<void(const TextInputEventArgs &)> func) = 0;
		virtual void AddTextEditCallback(std::function<void(const TextEditEventArgs &)> func)	= 0;

		virtual void AddMousePressedCallback(std::function<void(const MouseButtonPressedEventArgs &)> func)	  = 0;
		virtual void AddMouseReleasedCallback(std::function<void(const MouseButtonReleasedEventArgs &)> func) = 0;
		virtual void AddMouseMovedCallback(std::function<void(const MouseMovedEventArgs &)> func)			  = 0;
		virtual void AddMouseScrollCallback(std::function<void(const MouseScrolledEventArgs &)> func)		  = 0;
		virtual void AddMouseEnterCallback(std::function<void()> func)										  = 0;
		virtual void AddMouseLeaveCallback(std::function<void()> func)										  = 0;

		virtual void AddFileDropCallback(std::function<void(const FileDropEventArgs &)> func) = 0;

		virtual bool								   IsKeyDown(uint32_t keyboardId, ScanCode scancode)		 = 0;
		virtual bool								   IsKeyDown(ScanCode scancode)								 = 0;
		virtual std::pair<float, float>				   GetMousePosition(uint32_t mouseId)						 = 0;
		virtual std::optional<std::pair<float, float>> GetMousePosition()										 = 0;
		virtual std::pair<float, float>				   GetMouseScroll(uint32_t mouseId)							 = 0;
		virtual std::optional<std::pair<float, float>> GetMouseScroll()											 = 0;
		virtual bool								   IsMouseButtonPressed(uint32_t mouseId, MouseButton state) = 0;
		virtual bool								   IsMouseButtonPressed(MouseButton state)					 = 0;
	};
}	 // namespace Nexus