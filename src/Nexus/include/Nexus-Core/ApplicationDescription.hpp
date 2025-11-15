#pragma once

#include "Nexus-Core/Graphics/GraphicsAPICreateInfo.hpp"
#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Utils/Utils.hpp"
#include <inttypes.h>

namespace Nexus::Audio
{
	/// @brief An enum representing the different audio backends that are available
	enum AudioAPI
	{
		OpenAL
	};
}	 // namespace Nexus::Audio

namespace Nexus::Graphics
{
	/// @brief An enum class that describes how a swapchain will present the image onto the window
	enum class PresentMode
	{
		/// @brief The image will be presented immediately without waiting for the vertical blank, may result in tearing
		Immediate,

		/// @brief Presenting waits until the next vertical blank, tearing will not be observed. A single-entry queue is used to store the next image.
		Mailbox,

		/// @brief Presenting waits until the next vertical blank, tearing will not be observed. A multi-entry queue is used to store the next image.
		Fifo,

		/// @brief Presenting will wait until the next vertical blank, unless the vsync period has already elapsed, in which case the image will be
		/// presented immediately. May result in tearing
		FifoRelaxed
	};

	struct SwapchainDescription
	{
		/// @brief Whether the application should use VSync
		PresentMode ImagePresentMode = PresentMode::Fifo;

		/// @brief How many samples should be used by the swapchain
		uint32_t Samples = 0;
	};
}	 // namespace Nexus::Graphics

namespace Nexus
{
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

	enum WindowFlags : uint32_t
	{
		WindowFlags_None			  = 0,
		WindowFlags_Fullscreen		  = BIT(0),
		WindowFlags_Occluded		  = BIT(1),
		WindowFlags_Hidden			  = BIT(2),
		WindowFlags_Borderless		  = BIT(3),
		WindowFlags_Resizable		  = BIT(4),
		WindowFlags_Minimized		  = BIT(5),
		WindowFlags_Maximized		  = BIT(6),
		WindowFlags_MouseGrabbed	  = BIT(7),
		WindowFlags_InputFocus		  = BIT(8),
		WindowFlags_MouseFocus		  = BIT(9),
		WindowFlags_Modal			  = BIT(10),
		WindowFlags_HighPixelDensity  = BIT(11),
		WindowFlags_MouseCapture	  = BIT(12),
		WindowFlags_MouseRelativeMode = BIT(13),
		WindowFlags_AlwaysOnTop		  = BIT(14),
		WindowFlags_Utility			  = BIT(15),
		WindowFlags_Tooltip			  = BIT(16),
		WindowFlags_PopupMenu		  = BIT(17),
		WindowFlags_KeyboardGrabbed	  = BIT(18),
		WindowFlags_Transparent		  = BIT(19),
		WindowFlags_NotFocusable	  = BIT(20)
	};

	/// @brief A struct that represents a set of options for a window
	struct WindowDescription
	{
		/// @brief A string containing the title of the window
		std::string Title = "My Window";

		/// @brief An integer containing the width of the window
		uint32_t Width = 1280;

		/// @brief An integer containing the height of the window
		uint32_t Height = 720;

		uint32_t Flags = WindowFlags_None;

		std::optional<uint32_t> RendersPerSecond = {};
		std::optional<uint32_t> UpdatesPerSecond = {};
		std::optional<uint32_t> TicksPerSecond	 = {};

		std::string CanvasId = "canvas";
	};

	/// @brief A struct representing options to use when creating an application
	struct ApplicationDescription
	{
		/// @brief The graphics configuration to use when creating a GraphicsAPI and GraphicsDevice
		Graphics::GraphicsAPICreateInfo GraphicsCreateInfo;

		/// @brief The audio API to use to support sound effects
		Audio::AudioAPI AudioAPI;

		/// @brief Properties to configure the initial window
		WindowDescription WindowProperties;

		/// @brief Properties to configure the initial swapchain
		Graphics::SwapchainDescription SwapchainDescription;

		/// @brief Whether to create a default graphics queue using a queue with all available capabilities
		bool CreateDefaultGraphicsQueue = true;

		/// @brief Controls how the application will call Render(), Update() and Tick(), if true they will only be called following user input
		bool EventDriven = false;

		/// @brief The organization associated with the application, used for selecting a storage location
		std::string Organization = "Nexus";

		/// @brief The name associated with the application, used for selecting a storage location
		std::string App = "Application";
	};
}	 // namespace Nexus