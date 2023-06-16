#pragma once

#include "Shader.h"
#include "Viewport.h"

#include "Rectangle.h"

namespace Nexus
{
    enum class ComparisonFunction
    {
        Always,
        Equal,
        Greater,
        GreaterEqual,
        Less,
        LessEqual,
        Never,
        NotEqual
    };

    enum class BlendFunction
    {
        Zero,
        One,
        SourceColor,
        DestinationColor,
        OneMinusDestinationColor,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestinationAlpha,
        OneMinusDestinationAlpha,
    };

    enum class CullMode
    {
        Back,
        Front,
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

    enum StencilOperation
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

    struct PipelineDescription
    {
        DepthStencilDescription DepthStencilDescription;
        RasterizerStateDescription RasterizerStateDescription;
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