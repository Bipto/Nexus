#pragma once

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/FileSystem/FileDialogs.hpp"
#include "Nexus-Core/IWindow.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"
#include "Nexus-Core/Monitor.hpp"
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

	NX_API std::vector<InputNew::Keyboard> GetKeyboards();
	NX_API std::vector<InputNew::Mouse> GetMice();
	NX_API std::vector<InputNew::Gamepad> GetGamepads();
	NX_API std::vector<Monitor> GetMonitors();
	NX_API std::vector<IWindow *> &GetWindows();

	NX_API std::optional<InputNew::Keyboard> GetKeyboardById(uint32_t id);
	NX_API std::optional<InputNew::Mouse> GetMouseById(uint32_t id);
	NX_API std::optional<InputNew::Gamepad> GetGamepadById(uint32_t id);

	NX_API void		Initialise();
	NX_API void		Shutdown();
	NX_API void		Update();
	NX_API void		PollEvents();
	NX_API IWindow *CreatePlatformWindow(const WindowSpecification &windowProps);

	NX_API OpenFileDialog	*CreateOpenFileDialog(IWindow							  *window,
												  const std::vector<FileDialogFilter> &filters,
												  const char						  *defaultLocation,
												  bool								   allowMany);
	NX_API SaveFileDialog	*CreateSaveFileDialog(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation);
	NX_API OpenFolderDialog *CreateOpenFolderDialog(IWindow *window, const char *defaultLocation, bool allowMany);

	NX_API InputNew::MouseInfo GetGlobalMouseInfo();
	NX_API std::optional<IWindow *> GetKeyboardFocus();
	NX_API std::optional<IWindow *> GetMouseFocus();
	NX_API std::optional<IWindow *> GetActiveWindow();

	NX_API std::optional<uint32_t> GetActiveMouseId();
	NX_API std::optional<uint32_t> GetActiveKeyboardId();
	NX_API std::optional<uint32_t> GetActiveGamepadId();

	inline EventHandler<uint32_t> OnKeyboardAdded;
	inline EventHandler<uint32_t> OnKeyboardRemoved;
	inline EventHandler<uint32_t> OnMouseAdded;
	inline EventHandler<uint32_t> OnMouseRemoved;
	inline EventHandler<uint32_t> OnGamepadAdded;
	inline EventHandler<uint32_t> OnGamepadRemoved;

}	 // namespace Nexus::Platform