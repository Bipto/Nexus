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

    struct UpdateResourcesCommand
    {
        WeakRef<ResourceSet> Resources = {};
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

    struct SetRenderTargetCommand
    {
        RenderTarget Target = {};
    };

    struct SetViewportCommand
    {
        Viewport Viewport = {};
    };

    struct SetScissorCommand
    {
        Scissor Scissor = {};
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

    typedef std::variant<
        SetVertexBufferCommand,
        WeakRef<IndexBuffer>,
        WeakRef<Pipeline>,
        DrawElementCommand,
        DrawIndexedCommand,
        DrawInstancedCommand,
        DrawInstancedIndexedCommand,
        UpdateResourcesCommand,
        ClearColorTargetCommand,
        ClearDepthStencilTargetCommand,
        SetRenderTargetCommand,
        SetViewportCommand,
        SetScissorCommand,
        ResolveSamplesToSwapchainCommand,
        StartTimingQueryCommand,
        StopTimingQueryCommand>
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

        /// @brief A pure virutal method that begins a command list
        /// @param beginInfo A parameter containing information about how to begin the command list
        virtual void Begin() = 0;

        /// @brief A pure virtual method that ends a command list
        virtual void End() = 0;

        /// @brief A pure virtual method that binds a vertex buffer to the pipeline
        /// @param vertexBuffer A pointer to the vertex buffer to bind
        virtual void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer, uint32_t slot) = 0;

        /// @brief A pure virtual method that binds an index buffer to the pipeline
        /// @param indexBuffer A pointer to the index buffer to bind
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) = 0;

        /// @brief A pure virtual method to bind a pipeline to a command list
        /// @param pipeline The pointer to the pipeline to bind
        virtual void SetPipeline(Ref<Pipeline> pipeline) = 0;

        /// @brief A pure virtual method that submits a draw call using the bound vertex buffer
        /// @param start The offset to begin rendering at
        /// @param count The number of vertices to draw
        virtual void Draw(uint32_t start, uint32_t count) = 0;

        /// @brief A pure virtual method that submits an indexed draw call using the bound vertex buffer and index buffer
        /// @param count The number of vertices to draw
        /// @param offset The offset to begin rendering at
        virtual void DrawIndexed(uint32_t count, uint32_t indexStart, uint32_t vertexStart) = 0;

        /// @brief A pure virtual method that submits an instanced draw call using bound vertex buffers
        /// @param vertexCount The number of vertices to draw
        /// @param instanceCount The number of instances to draw
        /// @param vertexStart An offset into the vertex buffer to start rendering at
        /// @param instanceStart An offset into the instance buffer to start rendering at
        virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t instanceStart) = 0;

        /// @brief A pure virtual method that submits an instanced draw call using bound vertex buffers and an index buffer
        /// @param indexCount The number of indices in the primitive
        /// @param instanceCount The number of instances to draw
        /// @param vertexStart An offset into the vertex buffer to start rendering at
        /// @param indexStart An offset into the index buffer to start rendering at
        /// @param instanceStart An offset into the instance buffer to start rendering at
        virtual void DrawInstancedIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t vertexStart, uint32_t indexStart, uint32_t instanceStart) = 0;

        /// @brief A pure virtual method that updates the resources bound within a pipeline
        /// @param resources A reference counted pointer to a ResourceSet
        virtual void SetResourceSet(Ref<ResourceSet> resources) = 0;

        virtual void ClearColorTarget(uint32_t index, const ClearColorValue &color) = 0;

        virtual void ClearDepthTarget(const ClearDepthStencilValue &value) = 0;

        virtual void SetRenderTarget(RenderTarget target) = 0;

        virtual void SetViewport(const Viewport &viewport) = 0;

        virtual void SetScissor(const Scissor &scissor) = 0;

        virtual void ResolveFramebuffer(Ref<Framebuffer> source, uint32_t sourceIndex, Swapchain *target) = 0;

        virtual void StartTimingQuery(Ref<TimingQuery> query) = 0;

        virtual void StopTimingQuery(Ref<TimingQuery> query) = 0;
    };

    /// @brief A typedef to simplify creating function pointers to render commands
    typedef void (*RenderCommand)(Ref<CommandList> commandList);
}