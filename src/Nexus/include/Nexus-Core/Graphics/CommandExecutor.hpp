#pragma once

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	class NX_API CommandExecutor
	{
	  public:
		CommandExecutor() = default;
		virtual ~CommandExecutor() {};

	  protected:
		bool ValidateForGraphicsCall(std::optional<WeakRef<Pipeline>> pipeline, std::optional<RenderTarget> renderTarget);
		bool ValidateForComputeCall(std::optional<WeakRef<Pipeline>> pipeline);
		bool ValidateForClearColour(std::optional<RenderTarget> target, uint32_t colourIndex);
		bool ValidateForClearDepth(std::optional<RenderTarget> target);
		bool ValidateForSetViewport(std::optional<RenderTarget> target, const Viewport &viewport);
		bool ValidateForSetScissor(std::optional<RenderTarget> target, const Scissor &scissor);
		bool ValidateForResolveToSwapchain(const ResolveSamplesToSwapchainCommand &command);
	};
};	  // namespace Nexus::Graphics