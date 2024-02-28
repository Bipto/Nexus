#pragma once

#include "nxpch.hpp"
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "Point.hpp"
#include "Nexus/Events/Event.hpp"
#include "Nexus/Events/EventHandler.hpp"
#include "Nexus/Input/InputState.hpp"

#include "Nexus/Graphics/GraphicsAPI.hpp"
#include "Nexus/Graphics/Swapchain.hpp"
#include "backends/imgui_impl_sdl2.h"

#include "ApplicationSpecification.hpp"

namespace Nexus
{
    namespace Graphics
    {
        // forward declaration
        class GraphicsDevice;
    }

    /// @brief An enum representing the different default cursors that can be selected
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
        /// @param windowProps A structure containing options controlling how the window is created
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
        void SetSize(Point<uint32_t> size);

        /// @brief A method that closes a window
        void Close();

        /// @brief A method that checks whether a window is closing
        /// @return A boolean value indicating whether the window is closing
        bool IsClosing();

        /// @brief A method that returns the underlying SDL window handle
        /// @return  A pointer to an SDL window
        SDL_Window *GetSDLWindowHandle();

        /// @brief A method that returns the size of the window
        /// @return A Nexus::Point containing two integers representing the size of the window
        Point<uint32_t> GetWindowSize();

        /// @brief A method to get the position of the window
        /// @return A Nexus::Point containing two integers representing the position of the window
        Point<int> GetWindowPosition();

        /// @brief A method that returns the current state of the window
        /// @return A WindowState enum value representing the state of the window
        WindowState GetCurrentWindowState();

        /// @brief A method that toggles whether the mouse is visible within the window
        /// @param visible A boolean value indicating whether the mouse should be visible
        void SetIsMouseVisible(bool visible);

        /// @brief A method that sets the cursor used within the window
        /// @param cursor An enum value representing the cursor to use
        void SetCursor(Cursor cursor);

        /// @brief A method that returns the window's input state
        /// @return A pointer to the input state
        InputState *GetInput();

        /// @brief A method that checks whether a window is focussed
        /// @return A boolean value indicating whether the window is focussed
        bool IsFocussed();

        /// @brief A method that maximizes a window
        void Maximize();

        /// @brief A method that minimizes a window
        void Minimize();

        /// @brief A method that restores a window to it's previous size
        void Restore();

        /// @brief A method that toggles a window between fullscreen and windowed
        void ToggleFullscreen();

        /// @brief A method that returns a boolean value indicating whether the window is currently fullscreen or windowed
        /// @return A boolean indicating whether the window is fullscreen
        bool IsFullscreen();

        /// @brief A method that sets a window to be fullscreen
        void SetFullscreen();

        /// @brief A method that sets a window to be windowed
        void UnsetFullscreen();

        /// @brief A method that creates a new swapchain to support rendering to the window
        /// @param device The graphics device to use to create the swapchain
        /// @param vSyncState Whether or not vsync should be enabled for rendering
        void CreateSwapchain(Graphics::GraphicsDevice *device, const Graphics::SwapchainSpecification &swapchainSpec);

        /// @brief A method that returns a pointer to the window's swapchain
        /// @return A pointer to a swapchain
        Graphics::Swapchain *GetSwapchain();

        uint32_t GetID();

    private:
        /// @brief A method that returns a set of flags to use when creating the window
        /// @param api The graphics API to create the swapchain
        /// @return An unsigned int representing the flags
        uint32_t GetFlags(Graphics::GraphicsAPI api, const Graphics::SwapchainSpecification &swapchainSpec);

    private:
        /// @brief A pointer to the underlying SDL window
        SDL_Window *m_Window;

        /// @brief A boolean representing whether the window should close
        bool m_Closing = false;

        /// @brief A pointer to the window's input state
        InputState *m_Input;

        /// @brief A boolean value indicating whether the window is focussed
        bool m_IsFocussed = true;

        /// @brief An enum value representing the current state of the window
        WindowState m_CurrentWindowState = WindowState::Normal;

        /// @brief A pointer to the window's swapchain
        Graphics::Swapchain *m_Swapchain = nullptr;

        /// @brief A void pointer to the window's surface
        void *m_Surface = nullptr;

        /// @brief The underlying SDL window ID
        uint32_t m_WindowID = 0;

        /// @brief A friend class to allow an application to access private members of this class
        friend class Application;
    };
}