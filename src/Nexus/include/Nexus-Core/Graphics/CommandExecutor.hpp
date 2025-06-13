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
		virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) = 0;
		virtual void Reset()																				 = 0;

	  protected:
		bool ValidateForGraphicsCall(std::optional<WeakRef<Pipeline>> pipeline, std::optional<RenderTarget> renderTarget);
		bool ValidateForComputeCall(std::optional<WeakRef<Pipeline>> pipeline);
		bool ValidateForClearColour(std::optional<RenderTarget> target, uint32_t colourIndex);
		bool ValidateForClearDepth(std::optional<RenderTarget> target);
		bool ValidateForSetViewport(std::optional<RenderTarget> target, const Viewport &viewport);
		bool ValidateForSetScissor(std::optional<RenderTarget> target, const Scissor &scissor);
		bool ValidateForResolveToSwapchain(const ResolveSamplesToSwapchainCommand &command);

	  private:
		virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device)			   = 0;
		virtual void ExecuteCommand(SetIndexBufferCommand command, GraphicsDevice *device)			   = 0;
		virtual void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device)				   = 0;
		virtual void ExecuteCommand(DrawDescription command, GraphicsDevice *device)					= 0;
		virtual void ExecuteCommand(DrawIndexedDescription command, GraphicsDevice *device)				= 0;
		virtual void ExecuteCommand(DrawIndirectDescription command, GraphicsDevice *device)			= 0;
		virtual void ExecuteCommand(DrawIndirectIndexedDescription command, GraphicsDevice *device)		= 0;
		virtual void ExecuteCommand(DispatchDescription command, GraphicsDevice *device)				= 0;
		virtual void ExecuteCommand(DispatchIndirectDescription command, GraphicsDevice *device)		= 0;
		virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)				   = 0;
		virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device)		   = 0;
		virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device)	   = 0;
		virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device)					   = 0;
		virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device)				   = 0;
		virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device)					   = 0;
		virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device)  = 0;
		virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device)		   = 0;
		virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device)			   = 0;
		virtual void ExecuteCommand(SetStencilRefCommand command, GraphicsDevice *device)			   = 0;
		virtual void ExecuteCommand(SetDepthBoundsCommand command, GraphicsDevice *device)			   = 0;
		virtual void ExecuteCommand(SetBlendFactorCommand command, GraphicsDevice *device)				   = 0;
		virtual void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)  = 0;
		virtual void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device) = 0;
		virtual void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device) = 0;
		virtual void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device) = 0;
	};
};	  // namespace Nexus::Graphics