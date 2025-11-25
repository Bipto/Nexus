#pragma once

#include "Nexus-Core/Application.hpp"
#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/FileSystem/File.hpp"
#include "Nexus-Core/FileSystem/FileDialogs.hpp"
#include "Nexus-Core/FileSystem/Path.hpp"
#include "Nexus-Core/IWindow.hpp"
#include "Nexus-Core/Input/Events.hpp"
#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/Input.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"
#include "Nexus-Core/MessageBox.hpp"
#include "Nexus-Core/Monitor.hpp"
#include "Nexus-Core/Threading/Condition.hpp"
#include "Nexus-Core/Threading/Mutex.hpp"
#include "Nexus-Core/Threading/ReadWriteLock.hpp"
#include "Nexus-Core/Threading/Semaphore.hpp"
#include "Nexus-Core/Threading/Thread.hpp"
#include "Nexus-Core/Utils/SharedLibrary.hpp"

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

	NX_API void SetCursor(Cursor cursor);

	NX_API Utils::SharedLibrary *LoadSharedLibrary(const std::string &filename);

	NX_API std::vector<Keyboard> GetKeyboards();
	NX_API std::vector<Mouse> GetMice();
	NX_API std::vector<Gamepad> GetGamepads();
	NX_API std::vector<Monitor> GetMonitors();
	NX_API std::vector<IWindow *> &GetWindows();

	NX_API std::optional<Keyboard> GetKeyboardById(uint32_t id);
	NX_API std::optional<Mouse> GetMouseById(uint32_t id);
	NX_API std::optional<Gamepad> GetGamepadById(uint32_t id);

	NX_API void				 Initialise();
	NX_API void				 Shutdown();
	NX_API void				 Update();
	NX_API void				 PollEvents(Application *app);
	NX_API void				 WaitEvent(Application *app);
	NX_API IWindow			*CreatePlatformWindow(const WindowDescription &windowProps);
	NX_API MessageDialogBox *CreateMessageBox(const MessageBoxDescription &description);

	NX_API OpenFileDialog	*CreateOpenFileDialog(IWindow							  *window,
												  const std::vector<FileDialogFilter> &filters,
												  const char						  *defaultLocation,
												  bool								   allowMany);
	NX_API SaveFileDialog	*CreateSaveFileDialog(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation);
	NX_API OpenFolderDialog *CreateOpenFolderDialog(IWindow *window, const char *defaultLocation, bool allowMany);

	NX_API std::optional<IWindow *> GetKeyboardFocus();
	NX_API std::optional<IWindow *> GetMouseFocus();
	NX_API std::optional<IWindow *> GetActiveWindow();

	NX_API std::optional<uint32_t> GetActiveMouseId();
	NX_API std::optional<uint32_t> GetActiveKeyboardId();
	NX_API std::optional<uint32_t> GetActiveGamepadId();

	NX_API MouseState GetFocussedMouseState();
	NX_API MouseState GetGlobalMouseState();

	// filesystem
	NX_API const char *GetRootPath();
	NX_API const char *GetApplicationPath(const char *org, const char *app);
	NX_API std::string GetCurrentExecutableDirectory();
	NX_API std::string GetCurrentUserFolder(IO::UserFolder folder);

	NX_API std::string CopyFileTo(const char *source, const char *destination, bool overwriteIfExists);
	NX_API std::string CreateDirectoryAt(const char *path);
	NX_API IO::PathInfo GetPathInfo(const char *path);
	NX_API std::string RemovePath(const char *path);
	NX_API std::string RenamePath(const char *oldPath, const char *newPath);

	NX_API std::vector<std::string> EnumerateDirectoryContents(const char *path);

	enum class DelayAccuracy
	{
		Milliseconds,
		Nanoseconds,
		Precise
	};

	NX_API void Delay(TimeSpan timespan, DelayAccuracy accuracy);

	NX_API Threading::ThreadBase *CreateThreadBase(const Threading::ThreadDescription &description, std::function<void()> function);
	NX_API Threading::MutexBase *CreateMutexBase();
	NX_API Threading::ConditionBase *CreateConditionBase();
	NX_API Threading::SemaphoreBase *CreateSemaphoreBase(uint32_t startingValue);
	NX_API Threading::ReadWriteLockBase *CreateReadWriteLockBase();
	NX_API IO::FileStreamImpl *CreateFileStreamImpl(const std::filesystem::path &path, IO::FileMode fileMode);

	inline EventHandler<uint32_t> OnKeyboardAdded;
	inline EventHandler<uint32_t> OnKeyboardRemoved;
	inline EventHandler<uint32_t> OnMouseAdded;
	inline EventHandler<uint32_t> OnMouseRemoved;
	inline EventHandler<uint32_t> OnGamepadAdded;
	inline EventHandler<uint32_t> OnGamepadRemoved;

}	 // namespace Nexus::Platform