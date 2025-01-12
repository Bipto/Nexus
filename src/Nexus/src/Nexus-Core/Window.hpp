#pragma once

#include "Nexus-Core/nxpch.hpp"
#include "Platform/SDL3/SDL3Include.hpp"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include <emscripten/html5.h>
#endif

#if defined(NX_PLATFORM_WINDOWS)
	#include "Platform/Windows/WindowsInclude.hpp"
#endif

#if defined(NX_PLATFORM_LINUX)
	#include "Platform/X11/X11Include.hpp"
#endif

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
	namespace Graphics
	{
		// forward declaration
		class GraphicsDevice;
	}	 // namespace Graphics

	/// @brief A class representing a window
	class IWindow
	{
	  public:
		/// @brief A constructor taking in a window properties struct
		/// @param windowProps A structure containing options controlling how the
		/// window is created
		IWindow(const WindowSpecification &windowProps, Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec);

		/// @brief Copying a window is not supported
		/// @param A const reference to a window
		IWindow(const IWindow &) = delete;

		/// @brief A destructor to allow resources to be freed
		~IWindow();

		void CacheInput();

		void Update();

		/// @brief A method that allows a window to be resized
		/// @param isResizable Whether the window should be resizable
		void SetResizable(bool isResizable);

		/// @brief A method that sets the title of the window
		/// @param title A const reference to a string containing the new title
		void SetTitle(const std::string &title);

		/// @brief A method to set the size of a window
		/// @param size
		void SetSize(Point2D<uint32_t> size);

		/// @brief A method that closes a window
		void Close();

		/// @brief A method that checks whether a window is closing
		/// @return A boolean value indicating whether the window is closing
		bool IsClosing();

		/// @brief A method that returns the underlying SDL window handle
		/// @return  A pointer to an SDL window
		SDL_Window *GetSDLWindowHandle();

		/// @brief A method that returns the size of the window
		/// @return A Nexus::Point containing two integers representing the size of
		/// the window
		Point2D<uint32_t> GetWindowSize();

		/// @brief A method to get the position of the window
		/// @return A Nexus::Point containing two integers representing the position
		/// of the window
		Point2D<int> GetWindowPosition();

		/// @brief A method that returns the current state of the window
		/// @return A WindowState enum value representing the state of the window
		WindowState GetCurrentWindowState();

		/// @brief A method that toggles whether the mouse is visible within the
		/// window
		/// @param visible A boolean value indicating whether the mouse should be
		/// visible
		void SetIsMouseVisible(bool visible);

		/// @brief A method that returns the window's input state
		/// @return A pointer to the input state
		InputState *GetInput();

		Nexus::InputNew::InputContext *GetInputContext();

		/// @brief A method that checks whether a window is focussed
		/// @return A boolean value indicating whether the window is focussed
		bool IsFocussed();

		bool IsMinimized();

		bool IsMaximized();

		bool IsFullscreen();

		/// @brief A method that maximizes a window
		void Maximize();

		/// @brief A method that minimizes a window
		void Minimize();

		/// @brief A method that restores a window to it's previous size
		void Restore();

		/// @brief A method that toggles a window between fullscreen and windowed
		void ToggleFullscreen();

		/// @brief A method that sets a window to be fullscreen
		void SetFullscreen();

		/// @brief A method that sets a window to be windowed
		void UnsetFullscreen();

		void Show();

		void Hide();

		void SetWindowPosition(int32_t x, int32_t y);

		void Focus();

		uint32_t GetID();

		float GetDisplayScale();

		void SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect);
		void StartTextInput();
		void StopTextInput();

		void SetRendersPerSecond(uint32_t amount);
		void SetUpdatesPerSecond(uint32_t amount);
		void SetTicksPerSecond(uint32_t amount);
		void SetRelativeMouseMode(bool enabled);

		NativeWindowInfo GetNativeWindowInfo();

		EventHandler<const WindowResizedEventArgs &> OnResize;
		EventHandler<const WindowMovedEventArgs &>	 OnMove;

		EventHandler<> OnGainFocus;
		EventHandler<> OnLostFocus;
		EventHandler<> OnMaximized;
		EventHandler<> OnMinimized;
		EventHandler<> OnRestored;
		EventHandler<> OnShow;
		EventHandler<> OnHide;
		EventHandler<> OnExpose;

		EventHandler<const KeyPressedEventArgs &>  OnKeyPressed;
		EventHandler<const KeyReleasedEventArgs &> OnKeyReleased;

		EventHandler<const TextInputEventArgs &> OnTextInput;
		EventHandler<const TextEditEventArgs &>	 OnTextEdit;

		EventHandler<const MouseButtonPressedEventArgs &>  OnMousePressed;
		EventHandler<const MouseButtonReleasedEventArgs &> OnMouseReleased;
		EventHandler<const MouseMovedEventArgs &>		   OnMouseMoved;
		EventHandler<const MouseScrolledEventArgs &>	   OnScroll;
		EventHandler<>									   OnMouseEnter;
		EventHandler<>									   OnMouseLeave;

		EventHandler<const FileDropEventArgs &> OnFileDrop;

		EventHandler<TimeSpan> OnRender;
		EventHandler<TimeSpan> OnUpdate;
		EventHandler<TimeSpan> OnTick;

	  private:
		/// @brief A method that returns a set of flags to use when creating the
		/// window
		/// @param api The graphics API to create the swapchain
		/// @return An unsigned int representing the flags
		uint32_t GetFlags(Graphics::GraphicsAPI api, const WindowSpecification &windowSpec, const Graphics::SwapchainSpecification &swapchainSpec);

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

		InputNew::InputContext m_InputContext = {nullptr};
	};

	/* class IWindow
	{
	  public:
		virtual ~IWindow()
		{
		}

		virtual void SetPosition(int32_t x, int32_t y)		= 0;
		virtual void SetResizable(bool resizable)			= 0;
		virtual void SetTitle(const std::string &title)		= 0;
		virtual void SetSize(const Point2D<uint32_t> &size) = 0;
		virtual void SetMouseVisible(bool enabled)			= 0;
		virtual void SetRelativeMouse(bool enabled)			= 0;
		virtual void SetCursor(Cursor cursor)				= 0;

		virtual void SetRendersPerSecond(uint32_t time) = 0;
		virtual void SetUpdatesPerSecond(uint32_t time) = 0;
		virtual void SetTicksPerSecond(uint32_t time)	= 0;

		virtual InputState					 *GetInput()			  = 0;
		virtual const InputNew::InputContext &GetInputContext() const = 0;

		virtual bool IsFocussed() const	  = 0;
		virtual bool IsMinimised() const  = 0;
		virtual bool IsMaximised() const  = 0;
		virtual bool IsFullscreen() const = 0;

		virtual float			  GetDPI() const	  = 0;
		virtual Point2D<uint32_t> GetSize() const	  = 0;
		virtual Point2D<int>	  GetPosition() const = 0;
		virtual WindowState		  GetState() const	  = 0;

		virtual void		 Close()		   = 0;
		virtual bool		 IsClosing() const = 0;
		virtual WindowHandle GetHandle() const = 0;

		virtual void Maximise()					 = 0;
		virtual void Minimise()					 = 0;
		virtual void Restore()					 = 0;
		virtual void SetFullscreen(bool enabled) = 0;
		virtual void Show()						 = 0;
		virtual void Hide()						 = 0;
		virtual void Focus()					 = 0;

		virtual void				 CreateSwapchain(Graphics::GraphicsDevice *device, const Graphics::SwapchainSpecification &swapchainSpec) = 0;
		virtual Graphics::Swapchain *GetSwapchain()																							  = 0;

		virtual const WindowSpecification &GetSpecification() const = 0;

	  public:
		EventHandler<std::pair<uint32_t, uint32_t>> OnResize;

		EventHandler<> OnGainFocus;
		EventHandler<> OnLostFocus;
		EventHandler<> OnMaximized;
		EventHandler<> OnMinimized;
		EventHandler<> OnRestored;
		EventHandler<> OnShow;
		EventHandler<> OnHide;

		EventHandler<const KeyPressedEvent &>  OnKeyPressed;
		EventHandler<const KeyReleasedEvent &> OnKeyReleased;
		EventHandler<char *>				   OnTextInput;

		EventHandler<const MouseButtonPressedEvent &>  OnMousePressed;
		EventHandler<const MouseButtonReleasedEvent &> OnMouseReleased;
		EventHandler<const MouseMovedEvent &>		   OnMouseMoved;
		EventHandler<const MouseScrolledEvent &>	   OnScroll;
		EventHandler<>								   OnMouseEnter;
		EventHandler<>								   OnMouseLeave;

		EventHandler<const FileDropEvent &> OnFileDrop;

		EventHandler<TimeSpan> OnRender;
		EventHandler<TimeSpan> OnUpdate;
		EventHandler<TimeSpan> OnTick;
	}; */
}	 // namespace Nexus