#include "Nexus-Core/Graphics/CommandExecutor.hpp"

namespace Nexus::Graphics
{
	bool Nexus::Graphics::CommandExecutor::ValidateForGraphicsCall(std::optional<WeakRef<Pipeline>> pipeline,
																   std::optional<RenderTarget>		renderTarget)
	{
		bool valid = true;

		if (!renderTarget.has_value())
		{
			NX_ERROR("Attempting to execute graphics command without a bound render target");
			valid = false;
		}

		if (!pipeline.has_value())
		{
			NX_ERROR("Attempting to execute graphics command without a bound pipeline");
			valid = false;
		}

		return valid;
	}

	bool CommandExecutor::ValidateForComputeCall(std::optional<WeakRef<Pipeline>> pipeline)
	{
		if (!pipeline.has_value())
		{
			return false;
		}

		WeakRef<Pipeline> pl = pipeline.value();
		if (Ref<Pipeline> pipeline = pl.lock())
		{
			if (pipeline->GetType() != PipelineType::Compute)
			{
				return false;
			}
		}

		return true;
	}

	bool Nexus::Graphics::CommandExecutor::ValidateForClearColour(std::optional<RenderTarget> target, uint32_t colourIndex)
	{
		bool valid = true;

		if (!target.has_value())
		{
			NX_ERROR("Attempting to clear a colour target but no render target is bound");
			valid = false;
		}

		if (target.has_value())
		{
			RenderTarget t = target.value();
			if (colourIndex > t.GetColorAttachmentCount())
			{
				std::stringstream ss;
				ss << "Attempting to clear colour attachment at index: " << colourIndex << ", but render target contains "
				   << t.GetColorAttachmentCount() << " colour targets";
				NX_ERROR(ss.str());
				valid = false;
			}
		}

		return valid;
	}

	bool CommandExecutor::ValidateForClearDepth(std::optional<RenderTarget> target)
	{
		bool valid = true;

		if (!target.has_value())
		{
			NX_ERROR("Attempting to clear a depth/stencil target but none is bound");
			valid = false;
		}

		if (target.has_value())
		{
			RenderTarget t = target.value();
			if (!t.HasDepthAttachment())
			{
				NX_ERROR("Attempting to clear depth/stencil target but render target "
						 "does not contain depth attachment");
				valid = false;
			}
		}

		return valid;
	}

	bool CommandExecutor::ValidateForSetViewport(std::optional<RenderTarget> target, const Viewport &viewport)
	{
		bool valid = true;

		if (!target.has_value())
		{
			NX_ERROR("Attempting to set viewport but no render target has been specified");
			valid = false;
		}

		if (viewport.Width == 0)
		{
			NX_ERROR("Attempting to set a viewport with a width of 0");
			valid = false;
		}

		if (viewport.Height == 0)
		{
			NX_ERROR("Attempting to set a viewport with a height of 0");
			valid = false;
		}

		if (target.has_value())
		{
			RenderTarget t = target.value();

			if (viewport.X + viewport.Width > t.GetSize().X)
			{
				NX_ERROR("Attempting to set a viewport with a total width that is "
						 "greater than the width of the bound render target");
				valid = false;
			}

			if (viewport.Y + viewport.Height > t.GetSize().Y)
			{
				NX_ERROR("Attempting to set a viewport with a total height that is "
						 "greater than the height of the bound render target");
				valid = false;
			}
		}

		return valid;
	}

	bool CommandExecutor::ValidateForSetScissor(std::optional<RenderTarget> target, const Scissor &scissor)
	{
		bool valid = true;

		if (!target.has_value())
		{
			NX_ERROR("Attempting to set scissor but no render target has been specified");
			valid = false;
		}

		if (scissor.Width == 0)
		{
			NX_ERROR("Attempting to set a scissor with a width of 0");
			valid = false;
		}

		if (scissor.Height == 0)
		{
			NX_ERROR("Attempting to set a scissor with a height of 0");
			valid = false;
		}

		if (target.has_value())
		{
			RenderTarget t = target.value();

			if (scissor.X + scissor.Width > t.GetSize().X)
			{
				NX_ERROR("Attempting to set a scissor with a total width that is greater "
						 "than the width of the bound render target");
				valid = false;
			}

			if (scissor.Y + scissor.Height > t.GetSize().Y)
			{
				NX_ERROR("Attempting to set a scissor with a total height that is "
						 "greater than the height of the bound render target");
				valid = false;
			}
		}

		return valid;
	}

	bool CommandExecutor::ValidateForResolveToSwapchain(const ResolveSamplesToSwapchainCommand &command)
	{
		bool valid = true;

		if (!command.Source)
		{
			NX_ERROR("Attempting to resolve from an invalid framebuffer");
			valid = false;
		}

		if (!command.Target)
		{
			NX_ERROR("Attempting to resolve to an invalid swapchain");
			valid = false;
		}

		Ref<Framebuffer> source = command.Source;
		Ref<Swapchain>	 target = command.Target;

		if (source && target)
		{
			if (command.SourceIndex > source->GetColorTextureCount())
			{
				std::stringstream ss;
				ss << "Attempting to resolve from colour attachment: " << command.SourceIndex << " but supplied framebuffer has "
				   << source->GetColorTextureCount() << " colour attachments";
				NX_ERROR(ss.str());
				valid = false;
			}

			if (source->GetFramebufferSpecification().Width != target->GetSize().X)
			{
				std::stringstream ss;
				ss << "Attempting to resolve from a framebuffer to a swapchain of "
					  "mismatching widths. The width of the framebuffer is "
				   << source->GetFramebufferSpecification().Width << " and the width of the swapchain is " << target->GetSize().X;
				NX_ERROR(ss.str());
				valid = false;
			}

			if (source->GetFramebufferSpecification().Height != target->GetSize().Y)
			{
				std::stringstream ss;
				ss << "Attempting to resolve from a framebuffer to a swapchain of "
					  "mismatching heights. The height of the framebuffer is "
				   << source->GetFramebufferSpecification().Height << " and the height of the swapchain is " << target->GetSize().Y;
				NX_ERROR(ss.str());
				valid = false;
			}
		}

		return valid;
	}

}	 // namespace Nexus::Graphics
