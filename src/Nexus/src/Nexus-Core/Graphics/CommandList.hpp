#pragma once

#include "Color.hpp"
#include "Pipeline.hpp"
#include "GPUBuffer.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Nexus-Core/Types.hpp"
#include "RenderTarget.hpp"
#include "Viewport.hpp"
#include "Scissor.hpp"
#include "TimingQuery.hpp"
#include "ImageLayout.hpp"

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::Graphics
{
    struct SetVertexBufferCommand
    {
        WeakRef<VertexBuffer> VertexBufferRef = {};
        uint32_t Slot = 0;
    };

    /// @brief A struct representing a set of values to use  to clear the colour buffer
    struct ClearColorValue
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

    /// @brief A struct representing a set of values to use to clear the depth/stencil buffer
    struct ClearDepthStencilValue
    {
        /// @brief The value to use to clear the depth buffer
        float Depth = 1.0f;

        /// @brief The value to use to clear the stencil buffer
        uint8_t Stencil = 0;
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
        uint32_t IndexStart = 0;

        uint32_t VertexStart = 0;
    };

    struct DrawInstancedCommand
    {
        uint32_t VertexCount = 0;
        uint32_t InstanceCount = 0;
        uint32_t VertexStart = 0;
        uint32_t InstanceStart = 0;
    };

    struct DrawInstancedIndexedCommand
    {
        uint32_t IndexCount = 0;
        uint32_t InstanceCount = 0;
        uint32_t VertexStart = 0;
        uint32_t IndexStart = 0;
        uint32_t InstanceStart = 0;
    };

    struct ClearColorTargetCommand
    {
        uint32_t Index = {};
        ClearColorValue Color = {};
    };

    struct ClearDepthStencilTargetCommand
    {
        ClearDepthStencilValue Value = {};
    };

    struct ResolveSamplesToSwapchainCommand
    {
        WeakRef<Framebuffer> Source = {};
        uint32_t SourceIndex = {};
        Swapchain *Target = nullptr;
    };

    struct StartTimingQueryCommand
    {
        WeakRef<TimingQuery> Query = {};
    };

    struct StopTimingQueryCommand
    {
        WeakRef<TimingQuery> Query = {};
    };

    struct TransitionImageLayoutCommand
    {
        uint32_t BaseLevel = 0;
        uint32_t NumLevels = 0;
        WeakRef<Texture> TransitionTexture = {};
        ImageLayout TextureLayout = {};
    };

    typedef std::variant<
        SetVertexBufferCommand,
        WeakRef<IndexBuffer>,
        WeakRef<Pipeline>,
        DrawElementCommand,
        DrawIndexedCommand,
        DrawInstancedCommand,
        DrawInstancedIndexedCommand,
        Ref<ResourceSet>,
        ClearColorTargetCommand,
        ClearDepthStencilTargetCommand,
        RenderTarget,
        Viewport,
        Scissor,
        ResolveSamplesToSwapchainCommand,
        StartTimingQueryCommand,
        StopTimingQueryCommand,
        TransitionImageLayoutCommand>
        RenderCommandData;

    class CommandRecorder
    {
    public:
        CommandRecorder() = default;
        inline void Clear() { m_Commands.clear(); }
        inline void PushCommand(RenderCommandData command) { m_Commands.push_back(command); }
        inline const std::vector<RenderCommandData> &GetCommands() const { return m_Commands; }

    private:
        std::vector<RenderCommandData> m_Commands;
    };

    /// @brief A class representing a command list
    class CommandList
    {
    public:
        /// @brief A constructor creating a new command list
        CommandList() = default;

        /// @brief A virtual destructor allowing resources to be cleaned up
        virtual ~CommandList() {}

        /// @brief A method that begins a command list
        /// @param beginInfo A parameter containing information about how to begin the command list
        void Begin();

        /// @brief A method that ends a command list
        void End();

        /// @brief A method that binds a vertex buffer to the pipeline
        /// @param vertexBuffer A pointer to the vertex buffer to bind
        void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot);

        /// @brief A method that binds an index buffer to the pipeline
        /// @param indexBuffer A pointer to the index buffer to bind
        void SetIndexBuffer(Ref<IndexBuffer> indexBuffer);

        /// @brief A method to bind a pipeline to a command list
        /// @param pipeline The pointer to the pipeline to bind
        void SetPipeline(Ref<Pipeline> pipeline);

        /// @brief A method that submits a draw call using the bound vertex buffer
        /// @param start The offset to begin rendering at
        /// @param count The number of vertices to draw
        void Draw(uint32_t start, uint32_t count);

        /// @brief A method that submits an indexed draw call using the bound vertex buffer and index buffer
        /// @param count The number of vertices to draw
        /// @param offset The offset to begin rendering at
        void DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart);

        /// @brief A method that submits an instanced draw call using bound vertex buffers
        /// @param vertexCount The number of vertices to draw
        /// @param instanceCount The number of instances to draw
        /// @param vertexStart An offset into the vertex buffer to start rendering at
        /// @param instanceStart An offset into the instance buffer to start rendering at
        void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart);

        /// @brief A method that submits an instanced draw call using bound vertex buffers and an index buffer
        /// @param indexCount The number of indices in the primitive
        /// @param instanceCount The number of instances to draw
        /// @param vertexStart An offset into the vertex buffer to start rendering at
        /// @param indexStart An offset into the index buffer to start rendering at
        /// @param instanceStart An offset into the instance buffer to start rendering at
        void DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart);

        /// @brief A method that updates the resources bound within a pipeline
        /// @param resources A reference counted pointer to a ResourceSet
        void SetResourceSet(Ref<ResourceSet> resources);

        void ClearColorTarget(uint32_t index, const ClearColorValue &color);

        void ClearDepthTarget(const ClearDepthStencilValue &value);

        void SetRenderTarget(RenderTarget target);

        void SetViewport(const Viewport &viewport);

        void SetScissor(const Scissor &scissor);

        void ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target);

        void StartTimingQuery(Ref<TimingQuery> query);

        void StopTimingQuery(Ref<TimingQuery> query);

        void TransitionImageLayout(Ref<Texture> texture, uint32_t baseLevel, uint32_t numLevels, ImageLayout layout);

        CommandRecorder &GetCommandRecorder();

    private:
        CommandRecorder m_CommandRecorder = {};
    };

    /// @brief A typedef to simplify creating function pointers to render commands
    typedef void (*RenderCommand)(Ref<CommandList> commandList);
}