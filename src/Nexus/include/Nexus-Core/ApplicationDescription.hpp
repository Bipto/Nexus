#pragma once

#include <inttypes.h>

#include "Nexus-Core/Graphics/GraphicsAPICreateInfo.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "Platform/IWindow.hpp"

#include "Nexus-Core/Utils/Utils.hpp"

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