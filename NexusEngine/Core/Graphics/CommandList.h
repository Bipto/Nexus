#pragma once

#include "Color.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Buffer.h"

#include <functional>

namespace Nexus
{
    /// @brief A struct representing a set of values to use  to clear the colour buffer
    struct ClearValue
    {
        /// @brief The red channel as a value between 0.0f and 1.0f
        float Red = 1.0f;

        /// @brief The green channel as a value between 0.0f and 1.0f
        float Green = 1.0f;

        /// @brief The blue channel as a value between 0.0f and 1.0f
        float Blue = 1.0f;

        /// @brief The alpha channel as a value between 0.0f and 1.0f
        float Alpha = 1.0f;
    };

    /// @brief A struct representing a set of values used to clear the buffers when beginning a command list
    struct CommandListBeginInfo
    {
        /// @brief An instance of a clear value to use to clear the colour buffer
        ClearValue ClearValue;

        /// @brief A floating point value to use to clear the depth buffer
        float DepthValue = 1.0f;

        /// @brief An unsigned 8 bit integer to use to clear the stencil buffer
        uint8_t StencilValue = 0;
    };

    /// @brief A struct representing a draw command to be executed using a vertex buffer
    struct DrawElementCommand
    {
        /// @brief The starting index within the vertex buffer
        uint32_t Start = 0;

        /// @brief The number of vertices to draw
        uint32_t Count = 0;
    };

    /// @brief A struct representing a draw command using a vertex buffer and an indexed buffer
    struct DrawIndexedCommand
    {
        /// @brief The number of vertices to draw
        uint32_t Count = 0;

        /// @brief An offset into the index buffer
        uint32_t Offset = 0;
    };

    /// @brief A struct representing a texture update command
    struct TextureUpdateCommand
    {
        /// @brief A pointer to the texture to update
        Ref<Texture> Texture;

        /// @brief A pointer to the shader to upload the texture to
        Ref<Shader> Shader;

        /// @brief The binding of the texture to update
        TextureBinding Binding;
    };

    /// @brief A struct representing a uniform buffer update command
    struct UniformBufferUpdateCommand
    {
        /// @brief A pointer to a uniform buffer to be updated
        Ref<UniformBuffer> Buffer;

        /// @brief A pointer to the data to be uploaded to the GPU
        void *Data;

        /// @brief The size of the data to be uploaded
        uint32_t Size;

        /// @brief An offset to upload the data to
        uint32_t Offset;
    };

    /// @brief A class representing a command list
    class CommandList
    {
    public:
        /// @brief A constructor creating a new command list
        /// @param pipeline A pipeline to be used when rendering with the command list
        CommandList(Ref<Pipeline> pipeline)
            : m_Pipeline(pipeline) {}

        /// @brief A pure virutal method that begins a command list
        /// @param beginInfo A parameter containing information about how to begin the command list
        virtual void Begin(const CommandListBeginInfo &beginInfo) = 0;

        /// @brief A pure virtual method that ends a command list
        virtual void End() = 0;

        /// @brief A pure virtual method that binds a vertex buffer to the pipeline
        /// @param vertexBuffer A pointer to the vertex buffer to bind
        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;

        /// @brief A pure virtual method that binds an index buffer to the pipeline
        /// @param indexBuffer A pointer to the index buffer to bind
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) = 0;

        /// @brief A pure virtual method that submits a draw call using the bound vertex buffer
        /// @param start The offset to begin rendering at
        /// @param count The number of vertices to draw
        virtual void DrawElements(uint32_t start, uint32_t count) = 0;

        /// @brief A pure virtual method that submits an indexed draw call using the bound vertex buffer and index buffer
        /// @param count The number of vertices to draw
        /// @param offset The offset to begin rendering at
        virtual void DrawIndexed(uint32_t count, uint32_t offset) = 0;

        /// @brief A pure virtual method to submit a texture update command
        /// @param texture A pointer to the texture to update
        /// @param shader A pointer requiring a texture update
        /// @param binding The binding slot of the texture
        virtual void UpdateTexture(Ref<Texture> texture, Ref<Shader> shader, const TextureBinding &binding) = 0;

        /// @brief A pure virtual method to submit a uniform buffer update command
        /// @param buffer A pointer to the buffer to upload data to
        /// @param data A pointer to the data to upload
        /// @param size The size of the data to be uploaded
        /// @param offset An offset to upload the data to
        virtual void UpdateUniformBuffer(Ref<UniformBuffer> buffer, void *data, uint32_t size, uint32_t offset) = 0;

        /// @brief A method to get the bound pipeline of the command list
        /// @return A const pointer to the currently bound pipeline
        Ref<Pipeline> GetPipeline() const
        {
            return m_Pipeline;
        }

        /// @brief A pure virtual method to get the clear colour value that the command list was started with
        /// @return The current clear colour value
        virtual const ClearValue &GetClearColorValue() = 0;

        /// @brief A pure virtual method to get the clear depth value that the command list was started with
        /// @return The current clear depth value
        virtual const float GetClearDepthValue() = 0;

        /// @brief A pure virtual method to get the clear stencil value that the command list was started with
        /// @return The current clear stencil value
        virtual const uint8_t GetClearStencilValue() = 0;

        /// @brief A pure virtual method to get the vertex buffers submitted to the command list
        /// @return A vector of pointers to the vertex buffers
        virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() = 0;

        /// @brief A method to get the index buffers submitted to the command list
        /// @return A vector of pointers to the index buffers
        virtual const std::vector<Ref<IndexBuffer>> &GetIndexBuffers() = 0;

        /// @brief A pure virtual method to get the vertex buffer used in the currently queued command
        /// @return The currently bound vertex buffer
        virtual Ref<VertexBuffer> GetCurrentVertexBuffer() = 0;

        /// @brief A pure virtual method to get the index buffer used in the currently queued command
        /// @return The currently bound index buffer
        virtual Ref<IndexBuffer> GetCurrentIndexBuffer() = 0;

        /// @brief A pure virtual method to get the currently queued draw element command
        /// @return The currently queued draw element command
        virtual DrawElementCommand &GetCurrentDrawElementCommand() = 0;

        /// @brief A pure virtual method to get the currently queued draw indexed command
        /// @return The currently queued draw indexed command
        virtual DrawIndexedCommand &GetCurrentDrawIndexedCommand() = 0;

        /// @brief A pure virtual method to get the currently queued texture update command
        /// @return The currently queued texture update command
        virtual TextureUpdateCommand &GetCurrentTextureUpdateCommand() = 0;

        /// @brief A pure virtual method to get the currently queued uniform buffer update command
        /// @return The currently queued uniform buffer update command
        virtual UniformBufferUpdateCommand &GetCurrentUniformBufferUpdateCommand() = 0;

    protected:
        /// @brief A variable containing the command list's currently bound pipeline
        Ref<Pipeline> m_Pipeline = nullptr;
    };

    /// @brief A typedef to simplify creating function pointers to render commands
    typedef void (*RenderCommand)(Ref<CommandList> commandList);
}