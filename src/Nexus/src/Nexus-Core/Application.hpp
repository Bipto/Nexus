#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Audio/AudioDevice.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Window.hpp"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "ApplicationSpecification.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/InputState.hpp"
#include "Nexus-Core/Monitor.hpp"
#include "Nexus-Core/Timings/Clock.hpp"
#include "Nexus-Core/Timings/Profiler.hpp"
#include "Nexus-Core/Timings/Timespan.hpp"
#include "Nexus-Core/Types.hpp"
#include "Point.hpp"

namespace Nexus
{
	/// @brief A static method to create a new audio device from a selected audio
	/// API
	/// @param api The audio API to use to manage audio resources
	/// @return A pointer to an audio device
	static Audio::AudioDevice *CreateAudioDevice(Audio::AudioAPI api);

	/// @brief A class representing an application
	class Application
	{
	  public:
		/// @brief A constructor taking in a specification
		/// @param spec The options to use when creating an application
		Application(const ApplicationSpecification &spec);

		/// @brief Copying an application is not supported
		/// @param A const reference to an application to copy
		Application(const Application &) = delete;

		/// @brief A virtual destructor allowing derived classes to clean up resources
		virtual ~Application();

		// required overridable methods
		/// @brief A pure virtual method that is called when the application is loaded
		virtual void Load() = 0;

		/// @brief A pure virtual method that is called every time that the
		/// application should update
		/// @param time The elapsed time since the last update
		virtual void Update(Nexus::TimeSpan time) = 0;

		/// @brief A pure virtual method that is called every time that the
		/// application should render
		/// @param time The elapsed time since the last render
		virtual void Render(Nexus::TimeSpan time) = 0;

		virtual void Tick(Nexus::TimeSpan time) {};

		/// @brief A pure virtual method that is called once the application is
		/// closing
		virtual void Unload() = 0;

		// optional overridable methods
		/// @brief A virtual method that is called when the application's window is
		/// resized
		/// @param size The new size of the window
		virtual void OnResize(Point2D<uint32_t> size)
		{
		}

		/// @brief A virtual method that allows a client to block the application from
		/// closing (e.g. to prompt to save)
		/// @return A boolean value representing whether the application should close
		virtual bool OnClose()
		{
			return true;
		}

		/// @brief A method that is used to run the update and render loops of the
		/// application
		void MainLoop();

		/// @brief A method that gets access to the application's window
		/// @return A handle to the application's main window
		Nexus::IWindow *GetPrimaryWindow();

		Nexus::Graphics::Swapchain *GetPrimarySwapchain();

		/// @brief A method that is used to retrieve the size of an application's
		/// window
		/// @return A Nexus::Point containing two integers representing the size of
		/// the window
		Point2D<uint32_t> GetWindowSize();

		/// @brief A method that is used to retrieve the location of a window
		/// @return A Nexus::Point containing two integers representing the position
		/// of the window
		Point2D<int> GetWindowPosition();

		/// @brief A method that returns whether the application's window has focus
		/// @return A boolean value representing whether the window is focussed
		bool IsWindowFocussed();

		/// @brief A method that returns the current state of the window
		/// @return A WindowState enum value representing the state of the window
		WindowState GetCurrentWindowState();

		/// @brief A method that toggles whether the mouse is visible
		/// @param visible A boolean value representing whether the mouse should be
		/// visible
		void SetIsMouseVisible(bool visible);

		/// @brief A method that closes the application
		void Close();

		/// @brief A method that returns whether the window should close
		/// @return A boolean value that represents whether a window should close
		bool ShouldClose();

		/// @brief A method that returns a pointer to the engine's core input state
		/// @return A pointer to an InputState
		const InputState *GetCoreInputState() const;

		/// @brief A method that returns a pointer to the application's graphics
		/// device
		/// @return A pointer to a graphics device
		Graphics::GraphicsDevice *GetGraphicsDevice();

		/// @brief A method that returns a pointer to the application's audio device
		/// @return A pointer to an audio device
		Audio::AudioDevice *GetAudioDevice();

		const Keyboard &GetGlobalKeyboardState() const;

	  protected:
		/// @brief A pointer to a graphics device
		Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;

		/// @brief A pointer to an audio device
		Audio::AudioDevice *m_AudioDevice = nullptr;

	  private:
		/// @brief The specification that the application was created with
		ApplicationSpecification m_Specification {};

		/// @brief A pointer to the application's main window
		Nexus::IWindow *m_Window = nullptr;

		Nexus::Graphics::Swapchain *m_Swapchain = nullptr;

		/// @brief A clock to time when renders and updates occur
		Clock m_Clock {};

		Keyboard m_GlobalKeyboardState {};
	};
}	 // namespace Nexus