#pragma once

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	/*struct DispatchTable
	{
		void (*SetVertexBufferCmd)(const SetVertexBufferCommand &, GraphicsDevice *);
		void (*SetIndexBufferCmd)(const SetIndexBufferCommand &, GraphicsDevice *);
		void (*SetPipelineCmd)(WeakRef<Pipeline>, GraphicsDevice *);
		void (*DrawCmd)(const DrawDescription &, GraphicsDevice *);
		void (*DrawIndexedCmd)(const DrawIndexedDescription &, GraphicsDevice *);
		void (*DrawIndirectCmd)(const DrawIndirectDescription &, GraphicsDevice *);
		void (*DrawIndirectIndexedCmd)(const DrawIndirectIndexedDescription &, GraphicsDevice *);
		void (*DispatchCmd)(const DispatchDescription *, GraphicsDevice *);
		void (*DispatchIndirectCmd)(const DispatchIndirectDescription &, GraphicsDevice *);
		void (*DrawMeshCmd)(const DrawMeshDescription &, GraphicsDevice *);
		void (*DrawMeshIndirectCmd)(const DrawMeshIndirectDescription &, GraphicsDevice *);
		void (*SetResourceSetCmd)(Ref<ResourceSet>, GraphicsDevice *);
		void (*ClearColourTargetCmd)(const ClearColorTargetCommand &, GraphicsDevice *);
		void (*ClearDepthStencilTargetCmd)(const ClearDepthStencilTargetCommand &, GraphicsDevice *);
		void (*SetRenderTargetCmd)(RenderTarget, GraphicsDevice *);
		void (*SetViewportCmd)(const Viewport &, GraphicsDevice *);
		void (*SetScissorCmd)(const Scissor &, GraphicsDevice *);
		void (*ResolveCmd)(const ResolveSamplesToSwapchainCommand &, GraphicsDevice *);
		void (*StartTimingQueryCmd)(const StartTimingQueryCommand &, GraphicsDevice *);
		void (*StopTimingQueryCmd)(const StopTimingQueryCommand &, GraphicsDevice *);
		void (*CopyBufferToBufferCmd)(const CopyBufferToBufferCommand &, GraphicsDevice *);
		void (*CopyBufferToTextureCmd)(const CopyBufferToTextureCommand &, GraphicsDevice *);
		void (*CopyTextureToBufferCmd)(const CopyTextureToBufferCommand &, GraphicsDevice *);
		void (*CopyTextureToTextureCmd)(const CopyTextureToTextureCommand &, GraphicsDevice *);
		void (*BeginDebugGroupCmd)(const BeginDebugGroupCommand &, GraphicsDevice *);
		void (*EndDebugGroupCmd)(const EndDebugGroupCommand &, GraphicsDevice *);
		void (*InsertDebugMarkerCmd)(const InsertDebugMarkerCommand &, GraphicsDevice *);
		void (*SetBlendFactorCmd)(const SetBlendFactorCommand &, GraphicsDevice *);
		void (*SetStencilReferenceCmd)(const SetStencilReferenceCommand &, GraphicsDevice *);
		void (*BuildAccelerationStructuresCmd)(const BuildAccelerationStructuresCommand &, GraphicsDevice *);
		void (*AccelerationStructureCopyCmd)(const AccelerationStructureCopyDescription &, GraphicsDevice *);
		void (*AccelerationStructureDeviceBufferCopyCmd)(const AccelerationStructureDeviceBufferCopyDescription &, GraphicsDevice *);
		void (*DeviceBufferAccelerationStructureCopyCmd)(const DeviceBufferAccelerationStructureCopyDescription &, GraphicsDevice *);
		void (*PushConstantsCmd)(const PushConstantsDesc &, GraphicsDevice *);
	};*/

	class NX_API CommandExecutor
	{
	  public:
		CommandExecutor() = default;
		virtual ~CommandExecutor() {};
		virtual void ExecuteCommands(Ref<CommandList> commandList, GraphicsDevice *device) = 0;
		virtual void Reset()															   = 0;

		bool ValidateForGraphicsCall(std::optional<WeakRef<Pipeline>> pipeline, std::optional<RenderTarget> renderTarget);
		bool ValidateForComputeCall(std::optional<WeakRef<Pipeline>> pipeline);
		bool ValidateForClearColour(std::optional<RenderTarget> target, uint32_t colourIndex);
		bool ValidateForClearDepth(std::optional<RenderTarget> target);
		bool ValidateForSetViewport(std::optional<RenderTarget> target, const Viewport &viewport);
		bool ValidateForSetScissor(std::optional<RenderTarget> target, const Scissor &scissor);
		bool ValidateForResolveToSwapchain(const ResolveSamplesToSwapchainCommand &command);

		virtual void ExecuteCommand(const SetVertexBufferCommand &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const SetIndexBufferCommand &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const SetPipelineCommand &command, GraphicsDevice *device)								 = 0;
		virtual void ExecuteCommand(const DrawDescription &command, GraphicsDevice *device)									 = 0;
		virtual void ExecuteCommand(const DrawIndexedDescription &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const DrawIndirectDescription &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const DrawIndirectIndexedDescription &command, GraphicsDevice *device)					 = 0;
		virtual void ExecuteCommand(const DispatchDescription &command, GraphicsDevice *device)								 = 0;
		virtual void ExecuteCommand(const DispatchIndirectDescription &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const DrawMeshDescription &command, GraphicsDevice *device)								 = 0;
		virtual void ExecuteCommand(const DrawMeshIndirectDescription &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device)										 = 0;
		virtual void ExecuteCommand(const ClearColourTargetCommand &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const ClearDepthStencilTargetCommand &command, GraphicsDevice *device)					 = 0;
		virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device)											 = 0;
		virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device)										 = 0;
		virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device)											 = 0;
		virtual void ExecuteCommand(const ResolveSamplesToSwapchainCommand &command, GraphicsDevice *device)				 = 0;
		virtual void ExecuteCommand(const StartTimingQueryCommand &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const StopTimingQueryCommand &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const CopyBufferToBufferCommand &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const CopyBufferToTextureCommand &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const CopyTextureToBufferCommand &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const CopyTextureToTextureCommand &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const BeginDebugGroupCommand &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const EndDebugGroupCommand &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const InsertDebugMarkerCommand &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const SetBlendFactorCommand &command, GraphicsDevice *device)							 = 0;
		virtual void ExecuteCommand(const SetStencilReferenceCommand &command, GraphicsDevice *device)						 = 0;
		virtual void ExecuteCommand(const BuildAccelerationStructuresCommand &command, GraphicsDevice *device)				 = 0;
		virtual void ExecuteCommand(const AccelerationStructureCopyDescription &command, GraphicsDevice *Device)			 = 0;
		virtual void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, GraphicsDevice *device) = 0;
		virtual void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, GraphicsDevice *device) = 0;
		virtual void ExecuteCommand(const PushConstantsDesc &command, GraphicsDevice *device)								 = 0;
	};
};	  // namespace Nexus::Graphics