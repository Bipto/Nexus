#pragma once

#include "Color.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"

#include <functional>
#include <variant>

namespace Nexus::Graphics
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
        CommandList() = default;

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

        /// @brief A pure virtual method to binds a pipeline to a command list
        /// @param pipeline The pointer to the pipeline to bind
        virtual void SetPipeline(Ref<Pipeline> pipeline) = 0;

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
    };

    /// @brief A typedef to simplify creating function pointers to render commands
    typedef void (*RenderCommand)(Ref<CommandList> commandList);

    typedef std::variant<
        CommandListBeginInfo,
        Ref<VertexBuffer>,
        Ref<IndexBuffer>,
        Ref<Pipeline>,
        TextureUpdateCommand,
        UniformBufferUpdateCommand,
        DrawElementCommand,
        DrawIndexedCommand>
        RenderCommandData;
}