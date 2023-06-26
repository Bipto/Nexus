#pragma once

#include "Core/Graphics/CommandList.h"

#include "DX11.h"

#include <array>

namespace Nexus
{
    class GraphicsDeviceDX11;

    class CommandListDX11 : public CommandList
    {
    public:
        CommandListDX11(GraphicsDeviceDX11 *graphicsDevice, Ref<Pipeline> pipeline);

        virtual void Begin(const CommandListBeginInfo &beginInfo) override;
        virtual void End() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding) override;
        virtual void UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset) override;

        virtual const ClearValue &GetClearColorValue() override;
        virtual const float GetClearDepthValue() override;
        virtual const uint8_t GetClearStencilValue() override;

        virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() override { return m_VertexBuffers; }
        virtual const std::vector<Ref<IndexBuffer>> &GetIndexBuffers() override { return m_IndexBuffers; }

        virtual Ref<VertexBuffer> GetCurrentVertexBuffer() override;
        virtual Ref<IndexBuffer> GetCurrentIndexBuffer() override;
        virtual DrawElementCommand &GetCurrentDrawElementCommand() override;
        virtual DrawIndexedCommand &GetCurrentDrawIndexedCommand() override;
        virtual TextureUpdateCommand &GetCurrentTextureUpdateCommand() override;
        virtual UniformBufferUpdateCommand &GetCurrentUniformBufferCommand() override;

#if defined(NX_PLATFORM_DX11)
        D3D11_PRIMITIVE_TOPOLOGY GetTopology();
#endif
        const std::array<RenderCommand, 1000> &GetRenderCommands();
        uint32_t GetCommandCount();

        GraphicsDeviceDX11 *GetGraphicsDevice() { return m_GraphicsDevice; }

    private:
        std::array<RenderCommand, 1000> m_Commands;
        uint32_t m_CommandIndex = 0;

        CommandListBeginInfo m_CommandListBeginInfo;

        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        std::vector<Ref<IndexBuffer>> m_IndexBuffers;
        std::vector<DrawElementCommand> m_ElementCommands;
        std::vector<DrawIndexedCommand> m_IndexedCommands;
        std::vector<TextureUpdateCommand> m_TextureUpdateCommands;
        std::vector<UniformBufferUpdateCommand> m_UniformBufferUpdateCommands;

        uint32_t m_VertexBufferIndex = 0;
        uint32_t m_IndexBufferIndex = 0;
        uint32_t m_ElementCommandIndex = 0;
        uint32_t m_IndexedCommandIndex = 0;
        uint32_t m_TextureCommandIndex = 0;
        uint32_t m_UniformBufferUpdateCommandIndex = 0;

        GraphicsDeviceDX11 *m_GraphicsDevice = nullptr;
    };
}