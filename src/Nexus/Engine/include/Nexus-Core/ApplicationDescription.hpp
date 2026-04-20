#pragma once

#include <inttypes.h>

#include "Audio/AudioAPI.hpp"

#include "Nexus-Core/nxpch.hpp"
#include "Platform/IWindow.hpp"
#include "RHI/GraphicsAPICreateInfo.hpp"
#include "RHI/ISurface.hpp"
#include "RHI/SwapchainDescription.hpp"

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