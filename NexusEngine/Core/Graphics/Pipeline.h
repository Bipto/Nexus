#pragma once

#include "Shader.h"
#include "Viewport.h"

#include "Rectangle.h"

namespace Nexus
{
    /// @brief An enum class representing how two pixels should be compared against each other
    enum class ComparisonFunction
    {
        /// @brief The test will always pass
        Always,

        /// @brief The test will pass if the values are equal
        Equal,

        /// @brief The test will pass if the value is greater than the value stored in the framebuffer
        Greater,

        /// @brief The test will pass if the value is greater or equal to the value stored in the framebuffer
        GreaterEqual,

        /// @brief The test will pass if the value is lesser than the value stored in the framebuffer
        Less,

        /// @brief The test will pass if the value is lesser than or equal to the value stored in the framebuffer
        LessEqual,

        /// @brief The test will never pass
        Never,

        /// @brief The test will pass if the values are not equal
        NotEqual
    };

    /// @brief An enum class representing how two values should be blended together
    enum class BlendFunction
    {
        /// @brief All channels will use a value of zero
        Zero,

        /// @brief All channels will use a value of one
        One,

        /// @brief All channels will use the source colour
        SourceColor,

        /// @brief All channels will use the inverse of the source data (1 - RGB)
        OneMinusSourceColor,

        /// @brief All channels will use the destination colour
        DestinationColor,

        /// @brief All channels will use the inverse of the destination data (1 - RGB)
        OneMinusDestinationColor,

        /// @brief All channels will use the alpha channel of the source
        SourceAlpha,

        /// @brief All channels will use the inverse of the alpha data (1 - A)
        OneMinusSourceAlpha,

        /// @brief All channels will use the alpha channel of the destination
        DestinationAlpha,

        /// @brief All channels will use the inverse of the desination alpha (1 - A)
        OneMinusDestinationAlpha,
    };

    /// @brief An enum class representing how values will be blended
    enum class BlendEquation
    {
        /// @brief Both values will be added together
        Add,

        /// @brief Subtract source 1 from source 2
        Subtract,

        /// @brief Subtract source 2 from source 1
        ReverseSubtract,

        /// @brief Find the minimum of both values
        Min,

        /// @brief Find the maximum of both values
        Max
    };

    /// @brief An enum class representing how faces will be culled
    enum class CullMode
    {
        /// @brief Back faces will not be rendered
        Back,

        /// @brief Front faces will not be rendered
        Front,

        /// @brief All faces will be rendered
        None
    };

    enum class FillMode
    {
        Solid,
        Wireframe
    };

    enum class FrontFace
    {
        Clockwise,
        CounterClockwise
    };

    enum class Topology
    {
        LineList,
        LineStrip,
        PointList,
        TriangleList,
        TriangleStrip
    };

    enum class StencilOperation
    {
        Keep,
        Zero,
        Replace,
        Increment,
        Decrement,
        Invert
    };

    struct DepthStencilDescription
    {
        bool EnableDepthWrite = false;
        bool EnableDepthTest = false;
        ComparisonFunction DepthComparisonFunction = ComparisonFunction::Never;
        bool EnableStencilTest = false;
        uint8_t StencilMask = 0xFF;
        ComparisonFunction StencilComparisonFunction = ComparisonFunction::Never;
        StencilOperation StencilFailOperation = StencilOperation::Keep;
        StencilOperation StencilSuccessDepthFailOperation = StencilOperation::Keep;
        StencilOperation StencilSuccessDepthSuccessOperation = StencilOperation::Keep;
    };

    struct RasterizerStateDescription
    {
        CullMode CullMode = CullMode::Back;
        FillMode FillMode = FillMode::Solid;
        FrontFace FrontFace = FrontFace::Clockwise;
        bool DepthClipEnabled = false;
        bool ScissorTestEnabled = false;
        Rectangle ScissorRectangle;
    };

    struct BlendStateDescription
    {
        bool EnableBlending = false;
        BlendFunction SourceColourBlend = BlendFunction::SourceColor;
        BlendFunction SourceAlphaBlend = BlendFunction::One;
        BlendFunction DestinationColourBlend = BlendFunction::DestinationColor;
        BlendFunction DestinationAlphaBlend = BlendFunction::DestinationAlpha;
        BlendEquation BlendEquation = BlendEquation::Add;
    };

    struct PipelineDescription
    {
        DepthStencilDescription DepthStencilDescription;
        RasterizerStateDescription RasterizerStateDescription;
        BlendStateDescription BlendStateDescription;
        Topology PrimitiveTopology = Topology::TriangleList;
        Ref<Shader> Shader;
        Viewport Viewport;
    };

    class Pipeline
    {
    public:
        Pipeline(const PipelineDescription &description)
        {
            m_Description = description;
        }
        Pipeline() = delete;
        virtual ~Pipeline() = default;
        virtual const PipelineDescription &GetPipelineDescription() const = 0;
        virtual Ref<Shader> GetShader() const { return m_Description.Shader; }

    protected:
        PipelineDescription m_Description;

    private:
        friend class GraphicsDevice;
    };
}