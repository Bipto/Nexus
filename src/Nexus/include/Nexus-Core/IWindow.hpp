#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "ApplicationSpecification.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Input/Events.hpp"
#include "Nexus-Core/Timings/Timer.hpp"
#include "Point.hpp"
#include "Utils/FramerateMonitor.hpp"

#include "Nexus-Core/Types.hpp"

#include "WindowInfo.hpp"

#include "Nexus-Core/Input/Input.hpp"

namespace Nexus
{

	/// @brief A class representing a window
	class IWindow
	{
	  public:
		/// @brief A constructor taking in a window properties struct
		/// @param windowProps A structure containing options controlling how the
		/// window is created
		IWindow(const WindowSpecification &windowProps)
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
		virtual void SetSize(Point2D<uint32_t> size) = 0;

		/// @brief A method that closes a window
		virtual void Close() = 0;

		/// @brief A method that checks whether a window is closing
		/// @return A boolean value indicating whether the window is closing
		virtual bool IsClosing() = 0;

		/// @brief A method that returns the size of the window in relative units
		/// @return A Nexus::Point containing two integers representing the size of
		/// the window
		virtual Point2D<uint32_t> GetWindowSize() = 0;

		/// @brief A method that returns the size of the window in pixels
		/// @return A Nexus::Point containing two integers representing the pixel size of the window
		virtual Point2D<uint32_t> GetWindowSizeInPixels() = 0;

		/// @brief A method to get the position of the window
		/// @return A Nexus::Point containing two integers representing the position
		/// of the window
		virtual Point2D<int> GetWindowPosition() = 0;

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
		virtual void SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect) = 0;

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

		virtual void SetResizeCallback(std::function<void(const WindowResizedEventArgs &)> func) = 0;
		virtual void SetMoveCallback(std::function<void(const WindowMovedEventArgs &)> func)	 = 0;

		virtual void SetFocusGainCallback(std::function<void()> func) = 0;
		virtual void SetFocusLostCallback(std::function<void()> func) = 0;
		virtual void SetMaximizedCallback(std::function<void()> func) = 0;
		virtual void SetMinimizedCallback(std::function<void()> func) = 0;
		virtual void SetRestoreCallback(std::function<void()> func)	  = 0;
		virtual void SetShowCallback(std::function<void()> func)	  = 0;
		virtual void SetHideCallback(std::function<void()> func)	  = 0;
		virtual void SetExposeCallback(std::function<void()> func)	  = 0;

		virtual void SetKeyPressedCallback(std::function<void(const KeyPressedEventArgs &)> func)	= 0;
		virtual void SetKeyReleasedCallback(std::function<void(const KeyReleasedEventArgs &)> func) = 0;

		virtual void SetTextInputCallback(std::function<void(const TextInputEventArgs &)> func) = 0;
		virtual void SetTextEditCallback(std::function<void(const TextEditEventArgs &)> func)	= 0;

		virtual void SetMousePressedCallback(std::function<void(const MouseButtonPressedEventArgs &)> func)	  = 0;
		virtual void SetMouseReleasedCallback(std::function<void(const MouseButtonReleasedEventArgs &)> func) = 0;
		virtual void SetMouseMovedCallback(std::function<void(const MouseMovedEventArgs &)> func)			  = 0;
		virtual void SetMouseScrollCallback(std::function<void(const MouseScrolledEventArgs &)> func)		  = 0;
		virtual void SetMouseEnterCallback(std::function<void()> func)										  = 0;
		virtual void SetMouseLeaveCallback(std::function<void()> func)										  = 0;

		virtual void SetFileDropCallback(std::function<void(const FileDropEventArgs &)> func) = 0;

		virtual bool		   IsKeyDown(uint32_t keyboardId, ScanCode scancode)		 = 0;
		virtual bool		   IsKeyDown(ScanCode scancode)								 = 0;
		virtual Point2D<float> GetMousePosition(uint32_t mouseId)						 = 0;
		virtual Point2D<float> GetMousePosition()										 = 0;
		virtual Point2D<float> GetMouseScroll(uint32_t mouseId)							 = 0;
		virtual Point2D<float> GetMouseScroll()											 = 0;
		virtual bool		   IsMouseButtonPressed(uint32_t mouseId, MouseButton state) = 0;
		virtual bool		   IsMouseButtonPressed(MouseButton state)					 = 0;
	};
}	 // namespace Nexus