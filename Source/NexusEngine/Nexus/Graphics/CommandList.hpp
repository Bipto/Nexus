#pragma once

#include "Color.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Framebuffer.hpp"
#include "Nexus/Types.hpp"
#include "RenderTarget.hpp"

#include <functional>
#include <variant>

namespace Nexus::Graphics
{
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
        uint32_t Offset = 0;
    };

    struct UpdateResourcesCommand
    {
        ResourceSet *Resources;
    };

    struct ClearColorTargetCommand
    {
        uint32_t Index;
        ClearColorValue Color;
    };

    struct ClearDepthStencilTargetCommand
    {
        ClearDepthStencilValue Value;
    };

    struct SetRenderTargetCommand
    {
        RenderTarget Target{};
    };

    struct SetViewportCommand
    {
        Viewport Viewport;
    };

    struct SetScissorCommand
    {
        Rectangle Scissor;
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
        virtual void SetVertexBuffer(VertexBuffer *vertexBuffer) = 0;

        /// @brief A pure virtual method that binds an index buffer to the pipeline
        /// @param indexBuffer A pointer to the index buffer to bind
        virtual void SetIndexBuffer(IndexBuffer *indexBuffer) = 0;

        /// @brief A pure virtual method to bind a pipeline to a command list
        /// @param pipeline The pointer to the pipeline to bind
        virtual void SetPipeline(Pipeline *pipeline) = 0;

        /// @brief A pure virtual method that submits a draw call using the bound vertex buffer
        /// @param start The offset to begin rendering at
        /// @param count The number of vertices to draw
        virtual void DrawElements(uint32_t start, uint32_t count) = 0;

        /// @brief A pure virtual method that submits an indexed draw call using the bound vertex buffer and index buffer
        /// @param count The number of vertices to draw
        /// @param offset The offset to begin rendering at
        virtual void DrawIndexed(uint32_t count, uint32_t offset) = 0;

        /// @brief A pure virtual method that updates the resources bound within a pipeline
        /// @param resources A reference counted pointer to a ResourceSet
        virtual void SetResourceSet(ResourceSet *resources) = 0;

        virtual void ClearColorTarget(uint32_t index, const ClearColorValue &color) = 0;

        virtual void ClearDepthTarget(const ClearDepthStencilValue &value) = 0;

        virtual void SetRenderTarget(RenderTarget target) = 0;

        virtual void SetViewport(const Viewport &viewport) = 0;

        virtual void SetScissor(const Rectangle &scissor) = 0;
    };

    /// @brief A typedef to simplify creating function pointers to render commands
    typedef void (*RenderCommand)(CommandList *commandList);

    typedef std::variant<
        VertexBuffer *,
        IndexBuffer *,
        Pipeline *,
        DrawElementCommand,
        DrawIndexedCommand,
        UpdateResourcesCommand,
        ClearColorTargetCommand,
        ClearDepthStencilTargetCommand,
        SetRenderTargetCommand,
        SetViewportCommand,
        SetScissorCommand>
        RenderCommandData;
}