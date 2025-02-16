#pragma once

#include "Nexus-Core/nxpch.hpp"

#include "Audio/AudioDevice.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "IWindow.hpp"

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
		NX_API Application(const ApplicationSpecification &spec);

		/// @brief Copying an application is not supported
		/// @param A const reference to an application to copy
		NX_API Application(const Application &) = delete;

		/// @brief A virtual destructor allowing derived classes to clean up resources
		NX_API virtual ~Application();

		// required overridable methods
		/// @brief A pure virtual method that is called when the application is loaded
		NX_API virtual void Load() = 0;

		/// @brief A pure virtual method that is called every time that the
		/// application should update
		/// @param time The elapsed time since the last update
		NX_API virtual void Update(Nexus::TimeSpan time) = 0;

		/// @brief A pure virtual method that is called every time that the
		/// application should render
		/// @param time The elapsed time since the last render
		NX_API virtual void Render(Nexus::TimeSpan time) = 0;

		NX_API virtual void Tick(Nexus::TimeSpan time) {};

		/// @brief A pure virtual method that is called once the application is
		/// closing
		NX_API virtual void Unload() = 0;

		// optional overridable methods
		/// @brief A virtual method that is called when the application's window is
		/// resized
		/// @param size The new size of the window
		NX_API virtual void OnResize(Point2D<uint32_t> size)
		{
		}

		/// @brief A virtual method that allows a client to block the application from
		/// closing (e.g. to prompt to save)
		/// @return A boolean value representing whether the application should close
		NX_API virtual bool OnClose()
		{
			return true;
		}

		/// @brief A method that is used to run the update and render loops of the
		/// application
		NX_API void MainLoop();

		/// @brief A method that gets access to the application's window
		/// @return A handle to the application's main window
		NX_API Nexus::IWindow *GetPrimaryWindow();

		NX_API Nexus::Graphics::Swapchain *GetPrimarySwapchain();

		/// @brief A method that is used to retrieve the size of an application's
		/// window
		/// @return A Nexus::Point containing two integers representing the size of
		/// the window
		NX_API Point2D<uint32_t> GetWindowSize();

		/// @brief A method that is used to retrieve the location of a window
		/// @return A Nexus::Point containing two integers representing the position
		/// of the window
		NX_API Point2D<int> GetWindowPosition();

		/// @brief A method that returns whether the application's window has focus
		/// @return A boolean value representing whether the window is focussed
		NX_API bool IsWindowFocussed();

		/// @brief A method that returns the current state of the window
		/// @return A WindowState enum value representing the state of the window
		NX_API WindowState GetCurrentWindowState();

		/// @brief A method that toggles whether the mouse is visible
		/// @param visible A boolean value representing whether the mouse should be
		/// visible
		NX_API void SetIsMouseVisible(bool visible);

		/// @brief A method that closes the application
		NX_API void Close();

		/// @brief A method that returns whether the window should close
		/// @return A boolean value that represents whether a window should close
		NX_API bool ShouldClose();

		/// @brief A method that returns a pointer to the engine's core input state
		/// @return A pointer to an InputState
		NX_API const InputState *GetCoreInputState() const;

		/// @brief A method that returns a pointer to the application's graphics
		/// device
		/// @return A pointer to a graphics device
		NX_API Graphics::GraphicsDevice *GetGraphicsDevice();

		/// @brief A method that returns a pointer to the application's audio device
		/// @return A pointer to an audio device
		NX_API Audio::AudioDevice *GetAudioDevice();

		NX_API const Keyboard &GetGlobalKeyboardState() const;

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