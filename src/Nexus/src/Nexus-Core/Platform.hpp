#pragma once

#include "Nexus-Core/Events/EventHandler.hpp"
#include "Nexus-Core/Input/Event.hpp"
#include "Nexus-Core/Input/Gamepad.hpp"
#include "Nexus-Core/Input/InputContext.hpp"
#include "Nexus-Core/Input/Keyboard.hpp"
#include "Nexus-Core/Input/Mouse.hpp"
#include "Nexus-Core/Monitor.hpp"
#include "Nexus-Core/Window.hpp"

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

	const char *GetSystemName();
	const char *GetBuildConfiguration();
	const char *GetProcessorType();

	void SetCursor(Cursor cursor);

	std::vector<InputNew::Keyboard> GetKeyboards();
	std::vector<InputNew::Mouse>	GetMice();
	std::vector<InputNew::Gamepad>	GetGamepads();
	std::vector<Monitor>			GetMonitors();

	std::optional<InputNew::Keyboard> GetKeyboardById(uint32_t id);
	std::optional<InputNew::Mouse>	  GetMouseById(uint32_t id);
	std::optional<InputNew::Gamepad>  GetGamepadById(uint32_t id);

	void	 Initialise();
	void	 Shutdown();
	void	 Update();
	IWindow *CreatePlatformWindow(const WindowSpecification				 &windowProps,
								  Graphics::GraphicsAPI					  api,
								  const Graphics::SwapchainSpecification &swapchainSpec);

	InputNew::MouseInfo		 GetGlobalMouseInfo();
	std::optional<IWindow *> GetKeyboardFocus();
	std::optional<IWindow *> GetMouseFocus();

	std::optional<uint32_t> GetActiveMouseId();
	std::optional<uint32_t> GetActiveKeyboardId();
	std::optional<uint32_t> GetActiveGamepadId();

	inline EventHandler<uint32_t> OnKeyboardAdded;
	inline EventHandler<uint32_t> OnKeyboardRemoved;
	inline EventHandler<uint32_t> OnMouseAdded;
	inline EventHandler<uint32_t> OnMouseRemoved;
	inline EventHandler<uint32_t> OnGamepadAdded;
	inline EventHandler<uint32_t> OnGamepadRemoved;
}	 // namespace Nexus::Platform