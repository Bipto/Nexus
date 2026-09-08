#pragma once

#include <optional>

#include "RHI/CommandList.hpp"
#include "RHI/GraphicsDevice.hpp"
#include "RHI/RHI-Core.hpp"
#include "RHI/Types.hpp"

namespace Nexus::Graphics
{
    using CommandFunc = void (*)(const CommandHeader *, CommandListStorage &, void *);

    class NX_RHI_API CommandExecutor
    {
      public:
        CommandExecutor() = default;
        virtual ~CommandExecutor() {};
        virtual void ExecuteCommands(ICommandList *commandList, IGraphicsDevice *device) = 0;
        virtual void Reset() = 0;

        bool ValidateForGraphicsCall(PipelineHandle pipeline, FramebufferHandle renderTarget);
        bool ValidateForComputeCall(PipelineHandle pipeline);
        bool ValidateForClearColour(FramebufferHandle target, uint32_t colourIndex);
        bool ValidateForClearDepth(FramebufferHandle target);
        bool ValidateForSetViewport(FramebufferHandle target, const Viewport &viewport);
        bool ValidateForSetScissor(FramebufferHandle target, const Scissor &scissor);
        bool ValidateForResolve(const ResolveTextureDescription &command);
    };
}; // namespace Nexus::Graphics