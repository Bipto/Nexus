#pragma once

#include <optional>

#include "RHI/CommandList.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/RHI-Core.hpp"
#include "RHI/Types.hpp"

namespace Nexus::Graphics
{
	class NX_RHI_API CommandExecutor
	{
	  public:
		CommandExecutor() = default;
		virtual ~CommandExecutor() {};
		virtual void ExecuteCommands(Ref<ICommandList> commandList, IGraphicsDevice *device) = 0;
		virtual void Reset()																 = 0;

		bool ValidateForGraphicsCall(PipelineHandle pipeline, FramebufferHandle renderTarget);
		bool ValidateForComputeCall(PipelineHandle pipeline);
		bool ValidateForClearColour(FramebufferHandle target, uint32_t colourIndex);
		bool ValidateForClearDepth(FramebufferHandle target);
		bool ValidateForSetViewport(FramebufferHandle target, const Viewport &viewport);
		bool ValidateForSetScissor(FramebufferHandle target, const Scissor &scissor);
		bool ValidateForResolve(const ResolveTextureDescription &command);

		virtual void ExecuteCommand(const SetVertexBufferCommand &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const SetIndexBufferCommand &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(PipelineHandle command, IGraphicsDevice *device)										  = 0;
		virtual void ExecuteCommand(const DrawDescription &command, IGraphicsDevice *device)								  = 0;
		virtual void ExecuteCommand(const DrawIndexedDescription &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const DrawIndirectDescription &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const DrawIndirectIndexedDescription &command, IGraphicsDevice *device)					  = 0;
		virtual void ExecuteCommand(const DispatchDescription &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const DispatchIndirectDescription &command, IGraphicsDevice *device)					  = 0;
		virtual void ExecuteCommand(const DrawMeshDescription &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const DrawMeshIndirectDescription &command, IGraphicsDevice *device)					  = 0;
		virtual void ExecuteCommand(const ResourceSetBindingDescription &desc, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const ClearColorTargetCommand &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const ClearDepthStencilTargetCommand &command, IGraphicsDevice *device)					  = 0;
		virtual void ExecuteCommand(FramebufferHandle command, IGraphicsDevice *device)										  = 0;
		virtual void ExecuteCommand(const Viewport &command, IGraphicsDevice *device)										  = 0;
		virtual void ExecuteCommand(const Scissor &command, IGraphicsDevice *device)										  = 0;
		virtual void ExecuteCommand(const ResolveTextureDescription &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const StartTimingQueryCommand &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const StopTimingQueryCommand &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const CopyBufferToBufferCommand &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const CopyBufferToTextureCommand &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const CopyTextureToBufferCommand &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const CopyTextureToTextureCommand &command, IGraphicsDevice *device)					  = 0;
		virtual void ExecuteCommand(const BeginDebugGroupCommand &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const EndDebugGroupCommand &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const InsertDebugMarkerCommand &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const SetBlendFactorCommand &command, IGraphicsDevice *device)							  = 0;
		virtual void ExecuteCommand(const SetStencilReferenceCommand &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const BuildAccelerationStructuresCommand &command, IGraphicsDevice *device)				  = 0;
		virtual void ExecuteCommand(const AccelerationStructureCopyDescription &command, IGraphicsDevice *Device)			  = 0;
		virtual void ExecuteCommand(const AccelerationStructureDeviceBufferCopyDescription &command, IGraphicsDevice *device) = 0;
		virtual void ExecuteCommand(const DeviceBufferAccelerationStructureCopyDescription &command, IGraphicsDevice *device) = 0;
		virtual void ExecuteCommand(const PushConstantsDesc &command, IGraphicsDevice *device)								  = 0;
		virtual void ExecuteCommand(const BarrierGroupDescription &command, IGraphicsDevice *device)						  = 0;
		virtual void ExecuteCommand(const TraceRaysDescription &desc, IGraphicsDevice *device)								  = 0;
		virtual void ExecuteCommand(const EndRenderingCommand &command, IGraphicsDevice *device)							  = 0;
	};
};	  // namespace Nexus::Graphics