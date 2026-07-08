#pragma once

#include "Platform/Events/EventHandler.hpp"
#include "Platform/Events/EventQueue.hpp"
#include "Platform/FileSystem/File.hpp"
#include "Platform/FileSystem/FileDialogs.hpp"
#include "Platform/FileSystem/Path.hpp"
#include "Platform/IWindow.hpp"
#include "Platform/Input/Events.hpp"
#include "Platform/Input/Gamepad.hpp"
#include "Platform/Input/Input.hpp"
#include "Platform/Input/Keyboard.hpp"
#include "Platform/Input/Mouse.hpp"
#include "Platform/Layers/LayerStack.hpp"
#include "Platform/MessageBox.hpp"
#include "Platform/Monitor.hpp"
#include "Platform/Platform-Core.hpp"
#include "Platform/Utils/SharedLibrary.hpp"

namespace Nexus::Platform
{
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

    NX_PLATFORM_API void SetCursor(Cursor cursor);

    NX_PLATFORM_API Utils::SharedLibrary *LoadSharedLibrary(const std::string &filename);

    NX_PLATFORM_API std::vector<Keyboard> GetKeyboards();
    NX_PLATFORM_API std::vector<Mouse> GetMice();
    NX_PLATFORM_API std::vector<Gamepad> GetGamepads();
    NX_PLATFORM_API std::vector<Monitor> GetMonitors();
    NX_PLATFORM_API std::vector<IWindow *> &GetWindows();

    NX_PLATFORM_API std::optional<Keyboard> GetKeyboardById(uint32_t id);
    NX_PLATFORM_API std::optional<Mouse> GetMouseById(uint32_t id);
    NX_PLATFORM_API std::optional<Gamepad> GetGamepadById(uint32_t id);

    NX_PLATFORM_API void Initialise();
    NX_PLATFORM_API void Shutdown();
    NX_PLATFORM_API void Update();
    NX_PLATFORM_API void PollEvents(Nexus::EventQueue &eventQueue);
    NX_PLATFORM_API void WaitEvent(Nexus::EventQueue &eventQueue);
    NX_PLATFORM_API IWindow *CreatePlatformWindow(const WindowDescription &windowProps);
    NX_PLATFORM_API MessageDialogBox *CreateMessageBox(const MessageBoxDescription &description);

    NX_PLATFORM_API OpenFileDialog *CreateOpenFileDialog(const OpenFileDialogDescription &desc);
    NX_PLATFORM_API SaveFileDialog *CreateSaveFileDialog(const SaveFileDialogDescription &desc);
    NX_PLATFORM_API OpenFolderDialog *CreateOpenFolderDialog(const OpenFolderDialogDescription &desc);

    NX_PLATFORM_API std::optional<IWindow *> GetKeyboardFocus();
    NX_PLATFORM_API std::optional<IWindow *> GetMouseFocus();
    NX_PLATFORM_API std::optional<IWindow *> GetActiveWindow();

    NX_PLATFORM_API std::optional<uint32_t> GetActiveMouseId();
    NX_PLATFORM_API std::optional<uint32_t> GetActiveKeyboardId();
    NX_PLATFORM_API std::optional<uint32_t> GetActiveGamepadId();

    NX_PLATFORM_API MouseState GetFocussedMouseState();
    NX_PLATFORM_API MouseState GetGlobalMouseState();

    // filesystem
    NX_PLATFORM_API std::string GetRootPath();
    NX_PLATFORM_API std::string GetApplicationPath(const std::string &org, const std::string &app);
    NX_PLATFORM_API std::string GetCurrentExecutableDirectory();
    NX_PLATFORM_API std::string GetCurrentUserFolder(IO::UserFolder folder);

    NX_PLATFORM_API std::string CopyFileTo(
        const std::string &source, const std::string &destination, bool overwriteIfExists
    );
    NX_PLATFORM_API std::string CreateDirectoryAt(const std::string &path);
    NX_PLATFORM_API IO::PathInfo GetPathInfo(const std::string &path);
    NX_PLATFORM_API std::string RemovePath(const std::string &path);
    NX_PLATFORM_API std::string RenamePath(const std::string &oldPath, const std::string &newPath);

    NX_PLATFORM_API std::vector<std::string> EnumerateDirectoryContents(const std::string &path);

    NX_PLATFORM_API bool AreAnyWindowsOpen();

    enum class DelayAccuracy
    {
        Milliseconds,
        Nanoseconds,
        Precise
    };

    NX_PLATFORM_API void Delay(TimeSpan timespan, DelayAccuracy accuracy);

    NX_PLATFORM_API IO::FileStreamImpl *CreateFileStreamImpl(const std::filesystem::path &path, IO::FileMode fileMode);

    inline EventHandler<uint32_t> OnKeyboardAdded;
    inline EventHandler<uint32_t> OnKeyboardRemoved;
    inline EventHandler<uint32_t> OnMouseAdded;
    inline EventHandler<uint32_t> OnMouseRemoved;
    inline EventHandler<uint32_t> OnGamepadAdded;
    inline EventHandler<uint32_t> OnGamepadRemoved;

} // namespace Nexus::Platform