#pragma once

#include "Nexus-Core/Graphics/Multisample.hpp"
#include "Nexus-Core/nxpch.hpp"

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
	/// @brief An enum class representing whether VSync is enabled
	enum class VSyncState
	{
		/// @brief The graphics card will render as many frames as possible or will be
		/// limited by the frame rate cap specified
		Disabled = 0,

		/// @brief The refresh rate will be synchronised to the monitor
		Enabled = 1
	};

	/// @brief An enum class that represents an available graphics API backend
	enum class GraphicsAPI
	{
		/// @brief Graphics will be created using OpenGL
		OpenGL,

		/// @brief Graphics will be created using Direct3D12
		D3D12,

		/// @brief Graphics will be created using Vulkan
		Vulkan
	};

	struct SwapchainSpecification
	{
		/// @brief Whether the application should use VSync
		Graphics::VSyncState VSyncState = Graphics::VSyncState::Enabled;

		/// @brief Whether the swapchain should be multisampled
		Graphics::SampleCount Samples = Graphics::SampleCount::SampleCount1;
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

	/// @brief A struct that represents a set of options for a window
	struct WindowSpecification
	{
		/// @brief A string containing the title of the window
		std::string Title = "My Window";

		/// @brief An integer containing the width of the window
		uint32_t Width = 1280;

		/// @brief An integer containing the height of the window
		uint32_t Height = 720;

		/// @brief A boolean indicating whether the window can be resized or not
		bool Resizable = true;

		/// @brief A boolean indicating whether the window should have a border
		bool Borderless = false;

		bool Utility = false;

		std::optional<uint32_t> RendersPerSecond = {};
		std::optional<uint32_t> UpdatesPerSecond = {};
		std::optional<uint32_t> TicksPerSecond	 = {};
	};

	/// @brief A struct representing options to use when creating an application
	struct ApplicationSpecification
	{
		/// @brief The graphics API to use for rendering
		Graphics::GraphicsAPI GraphicsAPI;

		/// @brief The audio API to use to support sound effects
		Audio::AudioAPI AudioAPI;

		/// @brief Properties to configure the initial window
		WindowSpecification WindowProperties;

		/// @brief Properties to configure the initial swapchain
		Graphics::SwapchainSpecification SwapchainSpecification;
	};
}	 // namespace Nexus