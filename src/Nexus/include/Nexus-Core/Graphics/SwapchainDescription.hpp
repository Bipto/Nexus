#pragma once

#include <optional>

#include "PixelFormat.hpp"

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
		Ref<ISurface> Surface = nullptr;

		/// @brief Whether the application should use VSync
		PresentMode ImagePresentMode = PresentMode::Fifo;

		uint32_t Width = 0;

		uint32_t Height = 0;

		PixelFormat ColourFormat = PixelFormat::R8_G8_B8_A8_UNorm;

		std::optional<PixelFormat> DepthFormat = std::nullopt;

		/// @brief How many samples should be used by the swapchain
		uint32_t Samples = 0;
	};
}	 // namespace Nexus::Graphics