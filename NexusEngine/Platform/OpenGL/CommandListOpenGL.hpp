#pragma once

#include "Core/Graphics/CommandList.hpp"
#include "Core/Graphics/GraphicsDevice.hpp"

#include "GL.hpp"

#include <array>

namespace Nexus::Graphics
{
    class CommandListOpenGL : public CommandList
    {
    public:
        CommandListOpenGL(GraphicsDevice *device) : m_Device(device) {}

        virtual void Begin(const CommandListBeginInfo &beginInfo) override;
        virtual void End() override;

        virtual void SetVertexBuffer(Ref<DeviceBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<DeviceBuffer> indexBuffer) override;
        virtual void SetPipeline(Ref<Pipeline> pipeline) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding) override;
        virtual void UpdateUniformBuffer(Ref<DeviceBuffer> buffer, void *data, uint32_t size, uint32_t offset) override;

        virtual const ClearValue &GetClearColorValue() override;
        virtual const float GetClearDepthValue() override;
        virtual const uint8_t GetClearStencilValue() override;

        virtual void BindNextPipeline() override;
        virtual DrawElementCommand &GetCurrentDrawElementCommand() override;
        virtual DrawIndexedCommand &GetCurrentDrawIndexedCommand() override;
        virtual TextureUpdateCommand &GetCurrentTextureUpdateCommand() override;
        virtual UniformBufferUpdateCommand &GetCurrentUniformBufferUpdateCommand() override;

        void *GetCurrentCommandData();

        GLenum GetTopology();
        const std::array<RenderCommand, 1000> &GetRenderCommands();
        uint32_t GetCommandCount();

    private:
        void SetCurrentPipeline(Pipeline *pipeline);

    private:
        GraphicsDevice *m_Device;
        std::array<RenderCommand, 1000> m_Commands;
        uint32_t m_CommandIndex = 0;

        CommandListBeginInfo m_CommandListBeginInfo;
        Pipeline *m_CurrentPipeline;

        std::vector<Ref<Pipeline>> m_Pipelines;
        std::vector<DrawElementCommand> m_ElementCommands;
        std::vector<DrawIndexedCommand> m_IndexedCommands;
        std::vector<TextureUpdateCommand> m_TextureUpdateCommands;
        std::vector<UniformBufferUpdateCommand> m_UniformBufferUpdateCommands;

        uint32_t m_VertexBufferIndex = 0;
        uint32_t m_IndexBufferIndex = 0;
        uint32_t m_PipelineIndex = 0;
        uint32_t m_ElementCommandIndex = 0;
        uint32_t m_IndexedCommandIndex = 0;
        uint32_t m_TextureCommandIndex = 0;
        uint32_t m_UniformBufferUpdateCommandIndex = 0;

        std::vector<void *> m_CommandData;
        uint32_t m_CommandDataIndex = 0;
    };
}