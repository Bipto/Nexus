#include <cstdint>
#include <iostream>
#include <map>
#include <optional>

#include "Platform/Platform.hpp"

#include "FileDialogsSDL3.hpp"
#include "Platform/Events/EventHandler.hpp"
#include "Platform/Events/EventQueue.hpp"
#include "Platform/Input/Events.hpp"
#include "Platform/Input/Input.hpp"
#include "Platform/Input/Keyboard.hpp"
#include "Platform/Input/Mouse.hpp"
#include "Platform/Platform.hpp"
#include "SDL3FileStream.hpp"
#include "SDL3Include.hpp"
#include "SDL3MessageBox.hpp"
#include "SDL3SharedLibrary.hpp"
#include "SDL3Window.hpp"

std::vector<Nexus::IWindow *> m_Windows = {};
std::vector<Nexus::IWindow *> m_WindowsToClose = {};

std::optional<uint32_t> m_ActiveMouse = {};
std::optional<uint32_t> m_ActiveKeyboard = {};
std::optional<uint32_t> m_ActiveGamepad = {};

std::map<Nexus::Platform::Cursor, SDL_Cursor *> m_Cursors;

static Nexus::IWindow *GetWindowFromHandle(uint32_t handle)
{
    for (int i = 0; i < m_Windows.size(); i++)
    {
        if (m_Windows[i]->GetID() == handle)
        {
            return m_Windows[i];
        }
    }
    return nullptr;
}

namespace Nexus::Platform
{
    void SetCursor(Cursor cursor)
    {
        SDL_SetCursor(m_Cursors[cursor]);
    }

    Utils::SharedLibrary *LoadSharedLibrary(const std::string &filename)
    {
        return new Utils::SharedLibrarySDL3(filename);
    }

    std::vector<Keyboard> GetKeyboards()
    {
        std::vector<Keyboard> keyboards;

        int count;
        SDL_KeyboardID *sdlKeyboards = SDL_GetKeyboards(&count);

        for (int i = 0; i < count; i++)
        {
            uint32_t id = sdlKeyboards[i];

            if (const char *instanceName = SDL_GetKeyboardNameForID(id))
            {
                std::string name = instanceName;
                keyboards.push_back(Keyboard(sdlKeyboards[i], name));
            }
            else
            {
                std::cout << SDL_GetError() << std::endl;
            }
        }

        SDL_free(sdlKeyboards);

        return keyboards;
    }

    std::vector<Mouse> GetMice()
    {
        std::vector<Mouse> mice;

        int count;
        SDL_MouseID *sdlMice = SDL_GetMice(&count);

        for (int i = 0; i < count; i++)
        {
            uint32_t id = sdlMice[i];

            if (const char *instanceName = SDL_GetMouseNameForID(id))
            {
                std::string name = instanceName;
                mice.push_back(Mouse(sdlMice[i], name));
            }
            else
            {
                throw std::runtime_error(SDL_GetError());
            }
        }

        SDL_free(sdlMice);

        return mice;
    }

    std::vector<Gamepad> GetGamepads()
    {
        std::vector<Gamepad> gamepads;

        int count;
        SDL_JoystickID *sdlGamepads = SDL_GetGamepads(&count);

        for (int i = 0; i < count; i++)
        {
            if (SDL_IsGamepad(sdlGamepads[i]))
            {
                uint32_t id = sdlGamepads[i];

                if (const char *instanceName = SDL_GetGamepadNameForID(id))
                {
                    std::string name = instanceName;
                    gamepads.push_back(Gamepad(sdlGamepads[i], name));
                }
            }
        }

        SDL_free(sdlGamepads);

        return gamepads;
    }

    std::vector<Monitor> GetMonitors()
    {
        std::vector<Monitor> monitors;

        int displayCount;
        SDL_DisplayID *displays = SDL_GetDisplays(&displayCount);

        for (int i = 0; i < displayCount; i++)
        {
            SDL_DisplayID id = displays[i];

            // Monitor monitor;
            float dpi = SDL_GetDisplayContentScale(id);
            std::string name = SDL_GetDisplayName(id);

            SDL_Rect displayBounds;
            SDL_GetDisplayBounds(id, &displayBounds);

            SDL_Rect usableBounds;
            SDL_GetDisplayUsableBounds(id, &usableBounds);

            const SDL_DisplayMode *displayMode = SDL_GetCurrentDisplayMode(id);

            Monitor monitor{
                .Position = {displayBounds.x, displayBounds.y},
                .Size = {displayBounds.w, displayBounds.h},
                .WorkPosition = {usableBounds.x, usableBounds.y},
                .WorkSize = {usableBounds.w, usableBounds.h},
                .DPI = dpi,
                .RefreshRate = displayMode->refresh_rate,
                .Name = name
            };

            monitors.push_back(monitor);
        }

        SDL_free(displays);

        return monitors;
    }

    std::vector<IWindow *> &GetWindows()
    {
        return m_Windows;
    }

    std::optional<Keyboard> GetKeyboardById(uint32_t id)
    {
        const std::vector<Keyboard> &keyboards = Platform::GetKeyboards();

        for (const auto &keyboard : keyboards)
        {
            if (keyboard.GetId() == id)
            {
                return keyboard;
            }
        }

        return {};
    }

    std::optional<Mouse> GetMouseById(uint32_t id)
    {
        const std::vector<Mouse> &mice = Platform::GetMice();

        for (const auto &mouse : mice)
        {
            if (mouse.GetId() == id)
            {
                return mouse;
            }
        }

        return {};
    }

    std::optional<Gamepad> GetGamepadById(uint32_t id)
    {
        const std::vector<Gamepad> &gamepads = Platform::GetGamepads();

        for (const auto &gamepad : gamepads)
        {
            if (gamepad.GetId() == id)
            {
                return gamepad;
            }
        }

        return {};
    }

    void CreateCursors(std::map<Cursor, SDL_Cursor *> &cursors)
    {
        m_Cursors[Cursor::Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
        m_Cursors[Cursor::IBeam] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
        m_Cursors[Cursor::Wait] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
        m_Cursors[Cursor::Crosshair] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
        m_Cursors[Cursor::WaitArrow] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
        m_Cursors[Cursor::ArrowNWSE] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
        m_Cursors[Cursor::ArrowNESW] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
        m_Cursors[Cursor::ArrowWE] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
        m_Cursors[Cursor::ArrowNS] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
        m_Cursors[Cursor::ArrowAllDir] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
        m_Cursors[Cursor::No] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
        m_Cursors[Cursor::Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    }

    void ReleaseCursors(std::map<Cursor, SDL_Cursor *> &cursors)
    {
        for (auto &pair : cursors)
        {
            SDL_DestroyCursor(pair.second);
        }
        cursors.clear();
    }

    void Initialise()
    {
        bool success =
            SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD);
        if (!success)
        {
            throw std::runtime_error(SDL_GetError());
        }

        SDL_SetHint(SDL_HINT_JOYSTICK_ENHANCED_REPORTS, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED, "1");

        CreateCursors(m_Cursors);
    }

    void Shutdown()
    {
        ReleaseCursors(m_Cursors);
        SDL_Quit();
    }

    void Update()
    {
        for (size_t i = 0; i < m_WindowsToClose.size(); i++)
        {
            IWindow *window = m_WindowsToClose[i];
            delete window;
            m_Windows.erase(
                std::remove(m_Windows.begin(), m_Windows.end(), window),
                m_Windows.end()
            );
            m_WindowsToClose.erase(
                std::remove(
                    m_WindowsToClose.begin(), m_WindowsToClose.end(), window
                ),
                m_WindowsToClose.end()
            );
            i--;
        }

        for (size_t i = 0; i < m_Windows.size(); i++)
        {
            IWindow *window = m_Windows[i];
            window->Update();

            if (window->IsClosing())
            {
                m_WindowsToClose.push_back(window);
            }
        }
    }

    static void HandleEvent(SDL_Event &event, EventQueue &eventQueue)
    {
        auto window = (SDL3Window *)GetWindowFromHandle(event.window.windowID);
        if (!window)
        {
            return;
        }

        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
        {
            auto nexusKeyCode =
                Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.key);
            auto nexusScanCode =
                Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.scancode);
            auto mods =
                Nexus::SDL3::GetNexusModifiersFromSDLModifiers(event.key.mod);

            KeyPressedEventArgs &keyPressedEvent =
                eventQueue.Add<KeyPressedEventArgs>();
            keyPressedEvent.KeyCode = nexusKeyCode;
            keyPressedEvent.ScanCode = nexusScanCode;
            keyPressedEvent.Repeat = event.key.repeat;
            keyPressedEvent.Unicode = event.key.raw;
            keyPressedEvent.Mods = mods;
            keyPressedEvent.KeyboardID = event.kdevice.which;

            m_ActiveKeyboard = event.kdevice.which;
            window->InvokeKeyPressedCallback(keyPressedEvent);

            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            auto nexusKeyCode =
                Nexus::SDL3::GetNexusKeyCodeFromSDLKeyCode(event.key.key);
            auto nexusScanCode =
                Nexus::SDL3::GetNexusScanCodeFromSDLScanCode(event.key.scancode);

            KeyReleasedEventArgs &keyReleasedEvent =
                eventQueue.Add<KeyReleasedEventArgs>();
            keyReleasedEvent.KeyCode = nexusKeyCode;
            keyReleasedEvent.ScanCode = nexusScanCode;
            keyReleasedEvent.Unicode = event.key.raw;
            keyReleasedEvent.KeyboardID = event.kdevice.which;

            m_ActiveKeyboard = event.kdevice.which;
            window->InvokeKeyReleasedCallback(keyReleasedEvent);

            break;
        }
        case SDL_EVENT_WINDOW_HIDDEN:
        {
            window->InvokeHideCallback();
            break;
        }
        case SDL_EVENT_WINDOW_SHOWN:
        {
            window->InvokeShowCallback();
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            auto [mouseType, mouseId] =
                Nexus::SDL3::GetMouseInfo(event.button.which);
            std::optional<Nexus::MouseButton> button =
                Nexus::SDL3::GetMouseButton(event.button.button);

            std::pair<int32_t, int32_t> windowPos = window->GetWindowPosition();
            float scale = window->GetDisplayScale();
            std::pair<float, float> localPos = {event.button.x, event.button.y};
            std::pair<float, float> screenPos = {
                event.button.x + windowPos.first, event.button.y + windowPos.second
            };

            if (button.has_value())
            {
                std::pair<int32_t, int32_t> windowPos = window->GetWindowPosition();

                MouseButtonPressedEventArgs &mousePressedEvent =
                    eventQueue.Add<MouseButtonPressedEventArgs>();
                mousePressedEvent.Button = button.value();
                mousePressedEvent.Position = localPos;
                mousePressedEvent.ScreenPosition = screenPos;
                mousePressedEvent.Clicks = event.button.clicks;
                mousePressedEvent.MouseID = mouseId;
                mousePressedEvent.Type = mouseType;

                m_ActiveMouse = mouseId;
                window->InvokeMousePressedCallback(mousePressedEvent);
            }

            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            auto [mouseType, mouseId] =
                Nexus::SDL3::GetMouseInfo(event.button.which);
            std::optional<Nexus::MouseButton> button =
                Nexus::SDL3::GetMouseButton(event.button.button);

            std::pair<int32_t, int32_t> windowPos = window->GetWindowPosition();
            float scale = window->GetDisplayScale();
            std::pair<float, float> localPos = {event.button.x, event.button.y};
            std::pair<float, float> screenPos = {
                event.button.x + windowPos.first, event.button.y + windowPos.second
            };

            if (button.has_value())
            {
                std::pair<int32_t, int32_t> windowPos = window->GetWindowPosition();

                MouseButtonReleasedEventArgs &mouseReleasedEvent =
                    eventQueue.Add<MouseButtonReleasedEventArgs>();
                mouseReleasedEvent.Button = button.value();
                mouseReleasedEvent.Position = localPos;
                mouseReleasedEvent.ScreenPosition = screenPos;
                mouseReleasedEvent.MouseID = mouseId;
                mouseReleasedEvent.Type = mouseType;

                m_ActiveMouse = mouseId;
                window->InvokeMouseReleasedCallback(mouseReleasedEvent);
            }

            break;
        }
        case SDL_EVENT_MOUSE_MOTION:
        {
            std::pair<int32_t, int32_t> windowPos = window->GetWindowPosition();
            float scale = window->GetDisplayScale();

            std::pair<float, float> localPos = {event.motion.x, event.motion.y};
            std::pair<float, float> movement = {
                event.motion.xrel, event.motion.yrel
            };
            std::pair<float, float> screenPos = {
                event.button.x + windowPos.first, event.button.y + windowPos.second
            };

            auto [mouseType, mouseId] =
                Nexus::SDL3::GetMouseInfo(event.motion.which);

            MouseMovedEventArgs &mouseMovedEvent =
                eventQueue.Add<MouseMovedEventArgs>();
            mouseMovedEvent.Position = localPos;
            mouseMovedEvent.ScreenPosition = screenPos;
            mouseMovedEvent.Movement = movement;
            mouseMovedEvent.MouseID = mouseId, mouseMovedEvent.Type = mouseType;

            m_ActiveMouse = mouseId;
            window->InvokeMouseMovedCallback(mouseMovedEvent);

            break;
        }
        case SDL_EVENT_MOUSE_WHEEL:
        {
            std::pair<int32_t, int32_t> windowPos = window->GetWindowPosition();
            float scale = window->GetDisplayScale();

            auto [mouseType, mouseId] = Nexus::SDL3::GetMouseInfo(event.wheel.which);
            Nexus::ScrollDirection direction =
                Nexus::SDL3::GetScrollDirection(event.wheel.direction);

            std::pair<float, float> screenPos = {
                event.button.x + windowPos.first, event.button.y + windowPos.second
            };

            Nexus::MouseScrolledEventArgs &scrollEvent =
                eventQueue.Add<MouseScrolledEventArgs>();
            scrollEvent.Scroll = {event.wheel.x, event.wheel.y};
            scrollEvent.Position = {event.wheel.mouse_x, event.wheel.mouse_y};
            scrollEvent.ScreenPosition = screenPos, scrollEvent.MouseID = mouseId,
            scrollEvent.Type = mouseType;
            scrollEvent.Direction = direction;

            m_ActiveMouse = mouseId;
            window->InvokeMouseScrollCallback(scrollEvent);

            break;
        }
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            window->Close();
            break;
        }
        case SDL_EVENT_FINGER_DOWN:
        {
            break;
        }
        case SDL_EVENT_FINGER_UP:
        {
            break;
        }
        case SDL_EVENT_FINGER_MOTION:
        {
            break;
        }
        case SDL_EVENT_FINGER_CANCELED:
        {
            break;
        }
        case SDL_EVENT_TEXT_INPUT:
        {
            TextInputEventArgs &args = eventQueue.Add<TextInputEventArgs>();
            args.Text = event.text.text;
            window->InvokeTextInputCallback(args);

            break;
        }
        case SDL_EVENT_TEXT_EDITING:
        {
            TextEditEventArgs &textEditArgs = eventQueue.Add<TextEditEventArgs>();
            textEditArgs.Text = event.edit.text;
            textEditArgs.Start = event.edit.start;
            textEditArgs.Length = event.edit.length;
            window->InvokeTextEditCallback(textEditArgs);

            break;
        }
        case SDL_EVENT_WINDOW_RESIZED:
        {
            WindowResizedEventArgs &resizeEventArgs =
                eventQueue.Add<WindowResizedEventArgs>();
            resizeEventArgs.Size = {
                (uint32_t)event.window.data1, (uint32_t)event.window.data2
            };
            window->InvokeResizeCallback(resizeEventArgs);

            break;
        }
        case SDL_EVENT_WINDOW_MOVED:
        {
            WindowMovedEventArgs &movedEventArgs =
                eventQueue.Add<WindowMovedEventArgs>();
            movedEventArgs.Position = {event.window.data1, event.window.data2};
            window->InvokeMoveCallback(movedEventArgs);

            break;
        }
        case SDL_EVENT_DROP_FILE:
        {
            Nexus::FileDropType type = Nexus::SDL3::GetFileDropType(event.drop.type);
            std::string sourceApp = {};
            std::string sourceData = {};

            std::pair<int32_t, int32_t> windowPos = window->GetWindowPosition();

            if (event.drop.source)
            {
                sourceApp = event.drop.source;
            }

            if (event.drop.data)
            {
                sourceData = event.drop.data;
            }

            float scale = window->GetDisplayScale();
            std::pair<float, float> localPos = {event.drop.x, event.drop.y};
            std::pair<float, float> screenPos = {
                event.drop.x + windowPos.first, event.drop.y + windowPos.second
            };

            FileDropEventArgs &fileDropEvent = eventQueue.Add<FileDropEventArgs>();
            fileDropEvent.Type = type;
            fileDropEvent.Position = localPos;
            fileDropEvent.ScreenPosition = screenPos;
            fileDropEvent.SourceApp = sourceApp;
            fileDropEvent.Data = sourceData;

            window->InvokeFileDropCallback(fileDropEvent);

            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        {
            window->InvokeFocusGainCallback();
            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        {
            window->InvokeFocusLostCallback();
            break;
        }
        case SDL_EVENT_WINDOW_MAXIMIZED:
        {
            window->InvokeMaximizedCallback();
            break;
        }
        case SDL_EVENT_WINDOW_MINIMIZED:
        {
            window->InvokeMinimizedCallback();
            break;
        }
        case SDL_EVENT_WINDOW_RESTORED:
        {
            window->InvokeRestoreCallback();
            break;
        }
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        {
            window->InvokeMouseEnterCallback();
            break;
        }
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        {
            window->InvokeMouseLeaveCallback();
            break;
        }
        case SDL_EVENT_WINDOW_EXPOSED:
        {
            window->InvokeExposeCallback();
            break;
        }
        case SDL_EVENT_KEYBOARD_ADDED:
        {
            Nexus::Platform::OnKeyboardAdded.Invoke(event.kdevice.which);
            break;
        }
        case SDL_EVENT_KEYBOARD_REMOVED:
        {
            Nexus::Platform::OnKeyboardRemoved.Invoke(event.kdevice.which);
            break;
        }
        case SDL_EVENT_MOUSE_ADDED:
        {
            Nexus::Platform::OnMouseAdded.Invoke(event.mdevice.which);
            break;
        }
        case SDL_EVENT_MOUSE_REMOVED:
        {
            Nexus::Platform::OnMouseRemoved.Invoke(event.mdevice.which);
            break;
        }
        case SDL_EVENT_GAMEPAD_ADDED:
        {
            Nexus::Platform::OnGamepadAdded.Invoke(event.cdevice.which);
            break;
        }
        case SDL_EVENT_GAMEPAD_REMOVED:
        {
            Nexus::Platform::OnGamepadRemoved.Invoke(event.cdevice.which);
            break;
        }
        }
    }

    void PollEvents(EventQueue &eventQueue)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            HandleEvent(event, eventQueue);
        }
    }

    void WaitEvent(EventQueue &eventQueue)
    {
        SDL_Event event;
        if (SDL_WaitEvent(&event))
        {
            HandleEvent(event, eventQueue);
        }
    }

    IWindow *CreatePlatformWindow(const WindowDescription &windowProps)
    {
        IWindow *window = new SDL3Window(windowProps);
        m_Windows.push_back(window);
        return window;
    }

    MessageDialogBox *CreateMessageBox(const MessageBoxDescription &description)
    {
        return new MessageBoxSDL3(description);
    }

    OpenFileDialog *CreateOpenFileDialog(const OpenFileDialogDescription &desc)
    {
        return new OpenFileDialogSDL3(desc);
    }

    SaveFileDialog *CreateSaveFileDialog(const SaveFileDialogDescription &desc)
    {
        return new SaveFileDialogSDL3(desc);
    }

    OpenFolderDialog *CreateOpenFolderDialog(const OpenFolderDialogDescription &desc)
    {
        return new OpenFolderDialogSDL3(desc);
    }

    std::optional<IWindow *> GetMouseFocus()
    {
        SDL_Window *focusWindow = SDL_GetMouseFocus();
        if (focusWindow == nullptr)
        {
            return {};
        }

        for (auto window : m_Windows)
        {
            SDL3Window *sdlWindow = (SDL3Window *)window;
            if (sdlWindow->GetSDLWindowHandle() == focusWindow)
            {
                return window;
            }
        }

        return {};
    }

    std::optional<IWindow *> GetActiveWindow()
    {
        for (auto window : m_Windows)
        {
            if (window->IsFocussed())
            {
                return window;
            }
        }

        return {};
    }

    std::optional<uint32_t> GetActiveMouseId()
    {
        return m_ActiveMouse;
    }

    std::optional<uint32_t> GetActiveKeyboardId()
    {
        return m_ActiveKeyboard;
    }

    std::optional<uint32_t> GetActiveGamepadId()
    {
        return m_ActiveGamepad;
    }

    MouseState GetGlobalMouseState()
    {
        float x, y;

        Uint32 buttons = SDL_GetGlobalMouseState(&x, &y);

        MouseState state;
        state.MousePosition = {x, y};
        state.MouseWheel = {0.0f, 0.0f};
        state.LeftButton = MouseButtonState::Released;
        state.MiddleButton = MouseButtonState::Released;
        state.RightButton = MouseButtonState::Released;
        state.X1Button = MouseButtonState::Released;
        state.X2Button = MouseButtonState::Released;

        if (buttons & SDL_BUTTON_LEFT)
        {
            state.LeftButton = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_MIDDLE)
        {
            state.MiddleButton = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_RIGHT)
        {
            state.RightButton = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_X1)
        {
            state.X1Button = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_X2)
        {
            state.X2Button = MouseButtonState::Pressed;
        }

        return state;
    }

    MouseState GetFocussedMouseState()
    {
        float x, y;

        Uint32 buttons = SDL_GetMouseState(&x, &y);

        MouseState state;
        state.MousePosition = {x, y};
        state.MouseWheel = {0.0f, 0.0f};
        state.LeftButton = MouseButtonState::Released;
        state.MiddleButton = MouseButtonState::Released;
        state.RightButton = MouseButtonState::Released;
        state.X1Button = MouseButtonState::Released;
        state.X2Button = MouseButtonState::Released;

        if (buttons & SDL_BUTTON_LEFT)
        {
            state.LeftButton = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_MIDDLE)
        {
            state.MiddleButton = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_RIGHT)
        {
            state.RightButton = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_X1)
        {
            state.X1Button = MouseButtonState::Pressed;
        }

        if (buttons & SDL_BUTTON_X2)
        {
            state.X2Button = MouseButtonState::Pressed;
        }

        return state;
    }

    std::string GetRootPath()
    {
        return SDL_GetBasePath();
    }

    std::string GetApplicationPath(const std::string &org, const std::string &app)
    {
        return SDL_GetPrefPath(org.c_str(), app.c_str());
    }

    std::string GetCurrentExecutableDirectory()
    {
        char *path = SDL_GetCurrentDirectory();
        std::string directory = path;
        SDL_free(path);
        return directory;
    }

    static SDL_Folder GetUserFolderType(IO::UserFolder folder)
    {
        switch (folder)
        {
        case IO::UserFolder::Home:
            return SDL_FOLDER_HOME;
        case IO::UserFolder::Desktop:
            return SDL_FOLDER_DESKTOP;
        case IO::UserFolder::Documents:
            return SDL_FOLDER_DOCUMENTS;
        case IO::UserFolder::Downloads:
            return SDL_FOLDER_DOWNLOADS;
        case IO::UserFolder::Music:
            return SDL_FOLDER_MUSIC;
        case IO::UserFolder::Pictures:
            return SDL_FOLDER_PICTURES;
        case IO::UserFolder::PublicShare:
            return SDL_FOLDER_PUBLICSHARE;
        case IO::UserFolder::SavedGames:
            return SDL_FOLDER_SAVEDGAMES;
        case IO::UserFolder::Screenshots:
            return SDL_FOLDER_SCREENSHOTS;
        case IO::UserFolder::Teplates:
            return SDL_FOLDER_TEMPLATES;
        case IO::UserFolder::Videos:
            return SDL_FOLDER_VIDEOS;
        default:
            throw std::runtime_error("Failed to find a valid user folder type");
        }
    }

    std::string GetCurrentUserFolder(IO::UserFolder folder)
    {
        SDL_Folder sdlFolder = GetUserFolderType(folder);
        return SDL_GetUserFolder(sdlFolder);
    }

    std::string CopyFileTo(
        const std::string &source, const std::string &destination,
        bool overwriteIfExists
    )
    {
        if (!SDL_CopyFile(source.c_str(), destination.c_str()))
        {
            return std::string(SDL_GetError());
        }

        return {};
    }

    std::string CreateDirectoryAt(const std::string &path)
    {
        if (!SDL_CreateDirectory(path.c_str()))
        {
            return std::string(SDL_GetError());
        }

        return {};
    }

    IO::PathType GetPathTypeFromSDLPathType(SDL_PathType sdlType)
    {
        switch (sdlType)
        {
        case SDL_PATHTYPE_NONE:
            return IO::PathType::Invalid;
        case SDL_PATHTYPE_FILE:
            return IO::PathType::File;
        case SDL_PATHTYPE_DIRECTORY:
            return IO::PathType::Directory;
        case SDL_PATHTYPE_OTHER:
            return IO::PathType::Other;
        default:
            return IO::PathType::Invalid;
        }
    }

    IO::PathInfo GetPathInfo(const std::string &path)
    {
        IO::PathInfo info = {};

        SDL_PathInfo sdlPathInfo;
        if (SDL_GetPathInfo(path.c_str(), &sdlPathInfo))
        {
            info.Type = GetPathTypeFromSDLPathType(sdlPathInfo.type);
            info.SizeInBytes = sdlPathInfo.size;
            info.CreatedAt = DateTime::FromNanoseconds(sdlPathInfo.create_time);
            info.ModifiedAt = DateTime::FromNanoseconds(sdlPathInfo.modify_time);
            info.AccessedAt = DateTime::FromNanoseconds(sdlPathInfo.access_time);
        }

        return info;
    }

    std::string RemovePath(const std::string &path)
    {
        if (!SDL_RemovePath(path.c_str()))
        {
            return std::string(SDL_GetError());
        }

        return {};
    }

    std::string RenamePath(const std::string &oldPath, const std::string &newPath)
    {
        if (!SDL_RenamePath(oldPath.c_str(), newPath.c_str()))
        {
            return std::string(SDL_GetError());
        }

        return {};
    }

    static SDL_EnumerationResult IterateDirectory(
        void *userdata, const char *dirname, const char *fname
    )
    {
        auto files = static_cast<std::vector<std::string> *>(userdata);

        std::string fullpath = std::string(dirname) + "/" + std::string(fname);
        files->push_back(fullpath);

        return SDL_ENUM_CONTINUE;
    }

    std::vector<std::string> EnumerateDirectoryContents(const std::string &path)
    {
        std::vector<std::string> contents = {};
        SDL_EnumerateDirectory(path.c_str(), IterateDirectory, &contents);
        return contents;
    }

    bool AreAnyWindowsOpen()
    {
        return m_Windows.size() > 0;
    }

    void Delay(TimeSpan timespan, DelayAccuracy accuracy)
    {
        switch (accuracy)
        {
        case DelayAccuracy::Milliseconds:
        {
            SDL_Delay(timespan.GetMilliseconds<uint32_t>());
            break;
        }
        case DelayAccuracy::Nanoseconds:
        {
            SDL_DelayNS(timespan.GetNanoseconds<uint64_t>());
            break;
        }
        case DelayAccuracy::Precise:
        {
            SDL_DelayPrecise(timespan.GetNanoseconds<uint64_t>());
            break;
        }
        default:
            throw std::runtime_error("Failed to find a valid delay accuracy");
        }
    }

    IO::FileStreamImpl *CreateFileStreamImpl(
        const std::filesystem::path &path, IO::FileMode fileMode
    )
    {
        return new IO::FileStreamSDL3(path, fileMode);
    }

    std::optional<IWindow *> GetKeyboardFocus()
    {
        SDL_Window *focusWindow = SDL_GetKeyboardFocus();
        if (focusWindow == nullptr)
        {
            return {};
        }

        for (auto window : m_Windows)
        {
            SDL3Window *sdlWindow = (SDL3Window *)window;
            if (sdlWindow->GetSDLWindowHandle() == focusWindow)
            {
                return window;
            }
        }

        return {};
    }
} // namespace Nexus::Platform