#pragma once

#include "SDL.h"
#include "nxpch.hpp"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include <emscripten/html5.h>
#endif

#if defined(NX_PLATFORM_WINDOWS)
	#include "Platform/Windows/WindowsInclude.hpp"
#endif

#include "ApplicationSpecification.hpp"
#include "Nexus-Core/Events/Event.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Graphics/GraphicsAPI.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Graphics/Swapchain.hpp"
#include "Nexus-Core/Input/InputEvent.hpp"
#include "Nexus-Core/Input/InputState.hpp"
#include "Nexus-Core/Timings/Timer.hpp"
#include "Point.hpp"
#include "Utils/FramerateMonitor.hpp"

namespace Nexus
{
	namespace Graphics
	{
		// forward declaration
		class GraphicsDevice;
	}	 // namespace Graphics

	/// @brief An enum representing the different default cursors that can be
	/// selected
	enum class Cursor
	{
		Arrow,
		IBeam,
		Wait,
		Crosshair,
		WaitArrow,
		ArrowNWSE,
		ArrowNESW,
		ArrowWE,
		ArrowNS,
		ArrowAllDir,
		No,
		Hand
	};

	/// @brief A class representing a window
	class Window
	{
	  public:
		/// @brief A constructor taking in a window properties struct
		/// @param windowProps A structure containing options controlling how the
		/// window is created
		Window(const WindowSpecification &windowProps, Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec);

		/// @brief Copying a window is not supported
		/// @param A const reference to a window
		Window(const Window &) = delete;

		/// @brief A destructor to allow resources to be freed
		~Window();

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

		/// @brief A method that sets the cursor used within the window
		/// @param cursor An enum value representing the cursor to use
		void SetCursor(Cursor cursor);

		/// @brief A method that returns the window's input state
		/// @return A pointer to the input state
		const InputState *GetInput();

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

		/// @brief A method that creates a new swapchain to support rendering to the
		/// window
		/// @param device The graphics device to use to create the swapchain
		/// @param vSyncState Whether or not vsync should be enabled for rendering
		void CreateSwapchain(Graphics::GraphicsDevice *device, const Graphics::SwapchainSpecification &swapchainSpec);

		/// @brief A method that returns a pointer to the window's swapchain
		/// @return A pointer to a swapchain
		Graphics::Swapchain *GetSwapchain();

		uint32_t GetID();

		float GetDisplayScale();

		void SetTextInputRect(const Nexus::Graphics::Rectangle<int> &rect);
		void StartTextInput();
		void StopTextInput();

		void SetRendersPerSecond(uint32_t amount);
		void SetUpdatesPerSecond(uint32_t amount);
		void SetTicksPerSecond(uint32_t amount);
		void SetRelativeMouseMode(bool enabled);

		void OnEvent(const InputEvent &event);

#if defined(NX_PLATFORM_WINDOWS)
		const HWND GetHwnd() const;
#endif

		EventHandler<std::pair<uint32_t, uint32_t>> OnResize;

		EventHandler<std::string> OnFileDrop;
		EventHandler<>			  OnWindowGainFocus;
		EventHandler<>			  OnWindowLostFocus;
		EventHandler<>			  OnWindowMaximized;
		EventHandler<>			  OnWindowMinimized;
		EventHandler<>			  OnWindowRestored;

		EventHandler<Point2D<float>> OnMouseMoved;
		EventHandler<KeyCode>		 OnKeyPressed;
		EventHandler<KeyCode>		 OnKeyReleased;
		EventHandler<KeyCode>		 OnKeyHeld;

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

		/// @brief A pointer to the window's swapchain
		Graphics::Swapchain *m_Swapchain = nullptr;

		/// @brief A void pointer to the window's surface
		void *m_Surface = nullptr;

		/// @brief The underlying SDL window ID
		uint32_t m_WindowID = 0;

		Nexus::Timings::ExecutionTimer m_Timer = {};

		bool m_Minimized = false;

		Utils::FrameRateMonitor m_RenderFrameRateMonitor = {};
		Utils::FrameRateMonitor m_UpdateFrameRateMonitor = {};
		Utils::FrameRateMonitor m_TickFrameRateMonitor	 = {};

		/// @brief A friend class to allow an application to access private members of
		/// this class
		friend class Application;
	};
}	 // namespace Nexus