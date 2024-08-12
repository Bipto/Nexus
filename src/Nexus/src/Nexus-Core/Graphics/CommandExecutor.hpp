#pragma once

#include "Nexus-Core/Graphics/CommandList.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
    class CommandExecutor
    {
    public:
        CommandExecutor() = default;
        virtual ~CommandExecutor() {};
        virtual void ExecuteCommands(const std::vector<RenderCommandData> &commands, GraphicsDevice *device) = 0;
        virtual void Reset() = 0;

    protected:
        inline void SetImageLayout(Ref<Texture> texture, uint32_t level, ImageLayout layout)
        {
            texture->SetImageLayout(level, layout);
        }

        inline bool ValidateImageLayout(Ref<Texture> texture, uint32_t baseLevel, uint32_t numLevels, ImageLayout expectedLayout)
        {
            bool valid = true;

            for (uint32_t i = baseLevel; i < baseLevel + numLevels; i++)
            {
                std::optional<ImageLayout> textureLayout = texture->GetImageLayout(i);

                if (!textureLayout)
                {
                    NX_ERROR("Attempting to validate an invalid texture layer");
                    valid = false;
                }

                if (textureLayout.value() != expectedLayout)
                {
                    std::stringstream ss;
                    ss << "A level of the texture does not match the expected ImageLayout: ";
                    ss << "Expected level (" << i << ") to be in layout (" << ImageLayoutToString(expectedLayout);
                    ss << ") but was in layout: (" << ImageLayoutToString(textureLayout.value()) << ")";

                    NX_ERROR(ss.str());
                    valid = false;
                }
            }

            return valid;
        }

    private:
        virtual void ExecuteCommand(SetVertexBufferCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(WeakRef<IndexBuffer> command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(WeakRef<Pipeline> command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawElementCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawIndexedCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawInstancedCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(DrawInstancedIndexedCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(Ref<ResourceSet> command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(ClearColorTargetCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(ClearDepthStencilTargetCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(RenderTarget command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(const Viewport &command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(const Scissor &command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(ResolveSamplesToSwapchainCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(StartTimingQueryCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(StopTimingQueryCommand command, GraphicsDevice *device) = 0;
        virtual void ExecuteCommand(const TransitionImageLayoutCommand &command, GraphicsDevice *device) = 0;
    };
};