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

namespace Nexus
{
    namespace Graphics
    {
        // forward declaration
        class GraphicsDevice;
    }

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

    /// @brief A struct that represents a set of options for a window
    struct WindowProperties
    {
        /// @brief A string containing the title of the window
        std::string Title = "My Window";

        /// @brief An integer containing the width of the window
        int Width = 1280;

        /// @brief An integer containing the height of the window
        int Height = 720;

        /// @brief A boolean indicating whether the window can be resized or not
        bool Resizable = true;

        /// @brief An enum value representing the graphics API to use within the window
        Graphics::GraphicsAPI GraphicsAPI = Graphics::GraphicsAPI::None;
    };

    /// @brief A class representing a window
    class Window
    {
    public:
        /// @brief A constructor taking in a window properties struct
        /// @param windowProps A structure containing options controlling how the window is created
        Window(const WindowProperties &windowProps, Graphics::GraphicsAPI api);

        /// @brief Copying a window is not supported
        /// @param A const reference to a window
        Window(const Window &) = delete;

        /// @brief A destructor to allow resources to be freed
        ~Window();

        /// @brief A method to update the window's input
        /// @param imguiActiveA boolean value indicating whether the ImGui context needs updating
        void PollEvents(bool imguiActive = false);

        /// @brief A method that allows a window to be resized
        /// @param isResizable Whether the window should be resizable
        void SetResizable(bool isResizable);

        /// @brief A method that sets the title of the window
        /// @param title A const reference to a string containing the new title
        void SetTitle(const std::string &title);

        /// @brief A method to set the size of a window
        /// @param size
        void SetSize(Point<int> size);

        /// @brief A method that closes a window
        void Close();

        /// @brief A method that checks whether a window is closing
        /// @return A boolean value indicating whether the window is closing
        bool IsClosing();

        /// @brief A method that returns the underlying SDL window handle
        /// @return  A pointer to an SDL window
        SDL_Window *GetSDLWindowHandle();

        /// @brief A metho dthat returns the size of the window
        /// @return A Nexus::Point containing two integers representing the size of the window
        Point<int> GetWindowSize();

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

        void CreateSwapchain(Graphics::GraphicsDevice *device, Graphics::VSyncState vSyncState);

        Graphics::Swapchain *GetSwapchain();

    private:
        uint32_t GetFlags(Graphics::GraphicsAPI api);

    private:
        /// @brief A pointer to the underlying SDL window
        SDL_Window *m_Window;

        /// @brief A boolean representing whether the window should close
        bool m_Closing = false;

        /// @brief A boolean indicating whether the swapchain requires resizing
        bool m_SwapchainRequiresResize = false;

        /// @brief A pointer to the window's input state
        InputState *m_Input;

        /// @brief A boolean value indicating whether the window is focussed
        bool m_IsFocussed = true;

        /// @brief An enum value representing the current state of the window
        WindowState m_CurrentWindowState = WindowState::Normal;

        Graphics::Swapchain *m_Swapchain = nullptr;

        void *m_Surface = nullptr;

        /// @brief A friend class to allow an application to access private members of this class
        friend class Application;
    };
}