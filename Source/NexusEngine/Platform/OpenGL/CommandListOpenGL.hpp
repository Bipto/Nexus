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
        CommandListOpenGL(GraphicsDevice *device);

        virtual void Begin() override;
        virtual void End() override;

        virtual void BeginRenderPass(Ref<RenderPass> renderPass, const RenderPassBeginInfo &beginInfo) override;
        virtual void EndRenderPass() override;

        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;
        virtual void SetPipeline(Ref<Pipeline> pipeline) override;

        virtual void DrawElements(uint32_t start, uint32_t count) override;
        virtual void DrawIndexed(uint32_t count, uint32_t offset) override;

        virtual void UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureResourceBinding &binding) override;
        virtual void UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset) override;

    public:
        const std::vector<RenderCommand> &GetRenderCommands();
        RenderCommandData &GetCurrentCommandData();
        GLenum GetTopology();
        void BindPipeline(Ref<Pipeline> pipeline);
        GraphicsDevice *GetGraphicsDevice();

    private:
        GraphicsDevice *m_Device;
        std::vector<RenderCommand> m_Commands;
        std::vector<RenderCommandData> m_CommandData;
        uint32_t m_CommandIndex = 0;

        RenderPassBeginInfo m_CommandListBeginInfo;
        Ref<Pipeline> m_CurrentlyBoundPipeline;
    };
}