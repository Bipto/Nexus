#pragma once

#include "Audio/AudioDevice.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Platform/IWindow.hpp"
#include "Platform/Layers/LayerStack.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/IGraphicsAPI.hpp"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#include "ApplicationDescription.hpp"
#include "Platform/Events/EventQueue.hpp"
#include "Platform/Timings/Timespan.hpp"
#include "Point.hpp"
#include "RHI/Types.hpp"

namespace Nexus
{
	struct CommandQueueGroup
	{
		Ref<Graphics::ICommandQueue> GraphicsQueue = nullptr;
	};

	/// @brief A class representing an application
	class NX_API Application
	{
	  public:
		/// @brief A constructor taking in a specification
		/// @param spec The options to use when creating an application
		Application(const ApplicationDescription &spec);

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

		Ref<Nexus::Graphics::ISwapchain> GetPrimarySwapchain();

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

		/// @brief A method that returns a pointer to the application's graphics
		/// device
		/// @return A pointer to a graphics device
		Graphics::IGraphicsDevice *GetGraphicsDevice();

		Ref<Graphics::ICommandQueue> GetGraphicsCommandQueue();

		/// @brief A method that returns a pointer to the application's audio device
		/// @return A pointer to an audio device
		Audio::AudioDevice *GetAudioDevice();

		bool IsRunning() const;

		void Stop();

		std::string GetApplicationPath();

	  protected:
		std::unique_ptr<Graphics::IGraphicsAPI> m_GraphicsAPI = nullptr;

		/// @brief A pointer to a graphics device
		std::unique_ptr<Graphics::IGraphicsDevice> m_GraphicsDevice = nullptr;

		CommandQueueGroup m_CommandQueueGroup = {};

		/// @brief A pointer to an audio device
		std::shared_ptr<Audio::AudioDevice> m_AudioDevice = nullptr;

		LayerStack m_LayerStack = {};

		EventQueue m_EventQueue = {};

	  private:
		/// @brief The specification that the application was created with
		ApplicationDescription m_Description {};

		/// @brief A pointer to the application's main window
		Nexus::IWindow *m_Window = nullptr;

		Ref<Nexus::Graphics::ISwapchain> m_Swapchain = nullptr;

		/// @brief A clock to time when renders and updates occur
		Clock m_Clock {};

		bool m_Running = true;
	};
}	 // namespace Nexus