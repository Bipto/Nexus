#pragma once

#include "nxpch.hpp"

#include "Window.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Audio/AudioDevice.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Point.hpp"

#include "Nexus/Events/Event.hpp"
#include "Nexus/Events/EventHandler.hpp"
#include "Nexus/Input/InputState.hpp"
#include "Nexus/Time.hpp"
#include "Nexus/Types.hpp"

#include "Nexus/Monitor.hpp"

#include "ApplicationSpecification.hpp"

#include "Nexus/nxpch.hpp"

namespace Nexus
{
    /// @brief A class represenging a clock to use for timings
    class Clock
    {
    public:
        Clock() = default;

        /// @brief A method allowing the clock to be updating to the currently elapsed time
        void Tick();

        /// @brief A method allowing the clock to return a structure representing the elapsed time
        /// @return A Nexus::Time object containing the currently elapsed time
        Nexus::Time GetTime();

    private:
        /// @brief A variable containing the time that the clock was created
        std::chrono::high_resolution_clock::time_point m_StartTime = std::chrono::high_resolution_clock::now();

        /// @brief A delta time representing how much time has elapsed since the clock was created
        uint64_t m_DeltaTime{};
    };

    /// @brief A static method to create a new graphics device from a set of options
    /// @param createInfo Options to use to configure the graphics device
    /// @return A pointer to a graphics device
    static Graphics::GraphicsDevice *CreateGraphicsDevice(const Graphics::GraphicsDeviceCreateInfo &createInfo, Window *window, const Graphics::SwapchainSpecification &swapchainSpec);

    /// @brief A static method to create a new audio device from a selected audio API
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

        /// @brief A pure virtual method that is called every time that the application should update
        /// @param time The elapsed time since the last update
        virtual void Update(Nexus::Time time) = 0;

        /// @brief A pure virtual method that is called every time that the application should render
        /// @param time The elapsed time since the last render
        virtual void Render(Nexus::Time time) = 0;

        /// @brief A pure virtual method that is called once the application is closing
        virtual void Unload() = 0;

        // optional overridable methods
        /// @brief A virtual method that is called when the application's window is resized
        /// @param size The new size of the window
        virtual void OnResize(Point2D<uint32_t> size) {}

        /// @brief A virtual method that allows a client to block the application from closing (e.g. to prompt to save)
        /// @return A boolean value representing whether the application should close
        virtual bool OnClose() { return true; }

        /// @brief A method that is used to run the update and render loops of the application
        void MainLoop();

        /// @brief A method that gets access to the application's window
        /// @return A handle to the application's main window
        Nexus::Window *GetPrimaryWindow();

        /// @brief A method that is used to retrieve the size of an application's window
        /// @return A Nexus::Point containing two integers representing the size of the window
        Point2D<uint32_t> GetWindowSize();

        /// @brief A method that is used to retrieve the location of a window
        /// @return A Nexus::Point containing two integers representing the position of the window
        Point2D<int> GetWindowPosition();

        /// @brief A method that returns whether the application's window has focus
        /// @return A boolean value representing whether the window is focussed
        bool IsWindowFocussed();

        /// @brief A method that returns the current state of the window
        /// @return A WindowState enum value representing the state of the window
        WindowState GetCurrentWindowState();

        /// @brief A method that toggles whether the mouse is visible
        /// @param visible A boolean value representing whether the mouse should be visible
        void SetIsMouseVisible(bool visible);

        /// @brief A method that sets the cursor style
        /// @param cursor An enum value representing the cursor style to use
        void SetCursor(Cursor cursor);

        /// @brief A method that closes the application
        void Close();

        /// @brief A method that returns whether the window should close
        /// @return A boolean value that represents whether a window should close
        bool ShouldClose();

        /// @brief A method that returns a pointer to a newly created window
        /// @param props A set of options to use to create a new window
        /// @param swapchainSpec A struct containing information about how to create a swapchain for the window
        /// @return A pointer to a new window
        Window *CreateApplicationWindow(const WindowSpecification &windowProps, const Graphics::SwapchainSpecification &swapchainSpec);

        /// @brief A method that returns a pointer to the engine's core input state
        /// @return A pointer to an InputState
        const InputState *GetCoreInputState() const;

        /// @brief A method that returns a pointer to the application's graphics device
        /// @return A pointer to a graphics device
        Graphics::GraphicsDevice *GetGraphicsDevice();

        /// @brief A method that returns a pointer to the application's audio device
        /// @return A pointer to an audio device
        Audio::AudioDevice *GetAudioDevice();

        static std::vector<Monitor> GetMonitors();

        EventHandler<char *> OnTextInput;

        const Keyboard &GetGlobalKeyboardState() const;

    private:
        void PollEvents();
        Window *GetWindowFromHandle(uint32_t handle);
        void CheckForClosingWindows();
        void CloseWindows();

    protected:
        /// @brief A pointer to a graphics device
        Graphics::GraphicsDevice *m_GraphicsDevice = nullptr;

        /// @brief A pointer to an audio device
        Audio::AudioDevice *m_AudioDevice = nullptr;

    private:
        /// @brief The specification that the application was created with
        ApplicationSpecification m_Specification{};

        /// @brief A pointer to the application's main window
        Nexus::Window *m_Window = nullptr;

        /// @brief A set of two unsignd integers containing the size of the window
        Point2D<uint32_t> m_PreviousWindowSize{};

        /// @brief An event handler for when the window is resized
        Nexus::EventHandler<Point2D<uint32_t>> m_WindowResizeEventHandler{};

        /// @brief A clock to time when renders and updates occur
        Clock m_Clock{};

        std::vector<Window *> m_Windows{};
        std::vector<std::pair<Window *, uint32_t>> m_WindowsToClose{};

        Keyboard m_GlobalKeyboardState{};
    };
}