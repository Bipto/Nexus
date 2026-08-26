#include "RHI/CommandExecutor.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace Nexus::Graphics
{
    bool Nexus::Graphics::CommandExecutor::ValidateForGraphicsCall(PipelineHandle pipeline,
                                                                   FramebufferHandle renderTarget)
    {
        bool valid = true;

        if (!renderTarget.IsValid())
        {
            throw std::runtime_error("Attempting to execute graphics command without a bound render "
                                     "target");
            valid = false;
        }

        if (!pipeline.IsValid())
        {
            throw std::runtime_error("Attempting to execute graphics command without a bound pipeline");
            valid = false;
        }

        return valid;
    }

    bool CommandExecutor::ValidateForComputeCall(PipelineHandle pipeline)
    {
        if (!pipeline.IsValid())
        {
            return false;
        }

        if (IPipeline *pl = pipeline.GetResource())
        {
            if (pl->GetType() != PipelineType::Compute)
            {
                return false;
            }
        }

        return true;
    }

    bool Nexus::Graphics::CommandExecutor::ValidateForClearColour(FramebufferHandle target, uint32_t colourIndex)
    {
        bool valid = true;

        if (!target.IsValid())
        {
            throw std::runtime_error("Attempting to clear a colour target but no render target is bound");
            valid = false;
        }

        if (target.IsValid())
        {
            if (colourIndex > target->GetColorTextureCount())
            {
                std::stringstream ss;
                ss << "Attempting to clear colour attachment at index: " << colourIndex
                   << ", but render target contains " << target->GetColorTextureCount() << " colour targets";
                throw std::runtime_error(ss.str());
                valid = false;
            }
        }

        return valid;
    }

    bool CommandExecutor::ValidateForClearDepth(FramebufferHandle target)
    {
        bool valid = true;

        if (!target.IsValid())
        {
            throw std::runtime_error("Attempting to clear a depth/stencil target but none is bound");
            valid = false;
        }

        if (target.IsValid())
        {
            if (!target->HasDepthTexture())
            {
                throw std::runtime_error("Attempting to clear depth/stencil target but render target "
                                         "does not contain depth attachment");
                valid = false;
            }
        }

        return valid;
    }

    bool CommandExecutor::ValidateForSetViewport(FramebufferHandle target, const Viewport &viewport)
    {
        bool valid = true;

        if (!target.IsValid())
        {
            throw std::runtime_error("Attempting to set viewport but no render target has been specified");
            valid = false;
        }

        if (viewport.Width == 0)
        {
            throw std::runtime_error("Attempting to set a viewport with a width of 0");
            valid = false;
        }

        if (viewport.Height == 0)
        {
            throw std::runtime_error("Attempting to set a viewport with a height of 0");
            valid = false;
        }

        if (target.IsValid())
        {
            auto [renderTargetWidth, renderTargetHeight] = target->GetSize();

            if (viewport.X + viewport.Width > renderTargetWidth)
            {
                throw std::runtime_error("Attempting to set a viewport with a total width that is "
                                         "greater than the width of the bound render target");
                valid = false;
            }

            if (viewport.Y + viewport.Height > renderTargetHeight)
            {
                throw std::runtime_error("Attempting to set a viewport with a total height that is "
                                         "greater than the height of the bound render target");
                valid = false;
            }
        }

        return valid;
    }

    bool CommandExecutor::ValidateForSetScissor(FramebufferHandle target, const Scissor &scissor)
    {
        bool valid = true;

        if (!target.IsValid())
        {
            throw std::runtime_error("Attempting to set scissor but no render target has been specified");
            valid = false;
        }

        if (scissor.Width == 0)
        {
            throw std::runtime_error("Attempting to set a scissor with a width of 0");
            valid = false;
        }

        if (scissor.Height == 0)
        {
            throw std::runtime_error("Attempting to set a scissor with a height of 0");
            valid = false;
        }

        if (target.IsValid())
        {
            if (scissor.X + scissor.Width > target->GetWidth())
            {
                throw std::runtime_error("Attempting to set a scissor with a total width that is greater "
                                         "than the width of the bound render target");
                valid = false;
            }

            if (scissor.Y + scissor.Height > target->GetHeight())
            {
                throw std::runtime_error("Attempting to set a scissor with a total height that is "
                                         "greater than the height of the bound render target");
                valid = false;
            }
        }

        return valid;
    }

    bool CommandExecutor::ValidateForResolve(const ResolveTextureDescription &command)
    {
        bool valid = true;

        if (!command.Source.IsValid())
        {
            throw std::runtime_error("Attempting to resolve from an invalid framebuffer");
            valid = false;
        }

        if (!command.Destination.IsValid())
        {
            throw std::runtime_error("Attempting to resolve to an invalid swapchain");
            valid = false;
        }

        TextureHandle source = command.Source;
        TextureHandle dest = command.Destination;
        if (source.IsValid() && dest.IsValid())
        {
            if (source->GetWidth() != dest->GetWidth())
            {
                std::stringstream ss;
                ss << "Attempting to resolve from a framebuffer to a swapchain of "
                      "mismatching widths. The width of the framebuffer is "
                   << source->GetWidth() << " and the width of the swapchain is " << dest->GetWidth();
                throw std::runtime_error(ss.str());
                valid = false;
            }

            if (source->GetHeight() != dest->GetHeight())
            {
                std::stringstream ss;
                ss << "Attempting to resolve from a framebuffer to a swapchain of "
                      "mismatching heights. The height of the framebuffer is "
                   << source->GetHeight() << " and the height of the swapchain is " << dest->GetHeight();
                throw std::runtime_error(ss.str());
                valid = false;
            }
        }

        return valid;
    }

} // namespace Nexus::Graphics
