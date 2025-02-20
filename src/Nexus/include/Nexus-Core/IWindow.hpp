#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "ApplicationSpecification.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/InputState.hpp"
#include "Nexus-Core/Timings/Timer.hpp"
#include "Point.hpp"
#include "Utils/FramerateMonitor.hpp"

#include "Nexus-Core/Types.hpp"

#include "WindowInfo.hpp"

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

		virtual void CacheInput() = 0;

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

		 /// @brief A method that returns the window's input state
		 /// @return A pointer to the input state
		 virtual InputState *GetInput() = 0;

		 virtual Nexus::InputNew::InputContext *GetInputContext() = 0;

		 /// @brief A method that checks whether a window is focussed
		 /// @return A boolean value indicating whether the window is focussed
		 virtual bool IsFocussed() = 0;

		 virtual bool IsMinimized() = 0;

		 virtual bool IsMaximized() = 0;

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

		 virtual void Show() = 0;

		 virtual void Hide() = 0;

		 virtual void SetWindowPosition(int32_t x, int32_t y) = 0;

		 virtual void Focus() = 0;

		 virtual uint32_t GetID() = 0;

		 virtual float GetDisplayScale() = 0;

		 virtual void SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect) = 0;
		 virtual void StartTextInput()												= 0;
		 virtual void StopTextInput()												= 0;

		 virtual void SetRendersPerSecond(uint32_t amount) = 0;
		 virtual void SetUpdatesPerSecond(uint32_t amount) = 0;
		 virtual void SetTicksPerSecond(uint32_t amount)   = 0;
		 virtual void SetRelativeMouseMode(bool enabled)   = 0;

		 virtual NativeWindowInfo GetNativeWindowInfo() = 0;

		 EventHandler<const WindowResizedEventArgs &> OnResize;
		 EventHandler<const WindowMovedEventArgs &>	  OnMove;

		 EventHandler<> OnGainFocus;
		 EventHandler<> OnLostFocus;
		 EventHandler<> OnMaximized;
		 EventHandler<> OnMinimized;
		 EventHandler<> OnRestored;
		 EventHandler<> OnShow;
		 EventHandler<> OnHide;
		 EventHandler<> OnExpose;

		 EventHandler<const KeyPressedEventArgs &>	OnKeyPressed;
		 EventHandler<const KeyReleasedEventArgs &> OnKeyReleased;

		 EventHandler<const TextInputEventArgs &> OnTextInput;
		 EventHandler<const TextEditEventArgs &>  OnTextEdit;

		 EventHandler<const MouseButtonPressedEventArgs &>	OnMousePressed;
		 EventHandler<const MouseButtonReleasedEventArgs &> OnMouseReleased;
		 EventHandler<const MouseMovedEventArgs &>			OnMouseMoved;
		 EventHandler<const MouseScrolledEventArgs &>		OnScroll;
		 EventHandler<>										OnMouseEnter;
		 EventHandler<>										OnMouseLeave;

		 EventHandler<const FileDropEventArgs &> OnFileDrop;

		 EventHandler<TimeSpan> OnRender;
		 EventHandler<TimeSpan> OnUpdate;
		 EventHandler<TimeSpan> OnTick;
	};
}	 // namespace Nexus