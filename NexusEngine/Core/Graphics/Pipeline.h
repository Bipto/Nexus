#pragma once

#include "Shader.h"
#include "Viewport.h"

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
        Both
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

    struct DepthStencilDescription
    {
        bool EnableDepthWrite = false;
        bool EnableDepthTest = false;
        ComparisonFunction ComparisonFunction = ComparisonFunction::Never;
    };

    struct RasterizerStateDescription
    {
        CullMode CullMode = CullMode::Back;
        FillMode FillMode = FillMode::Solid;
        FrontFace FrontFace = FrontFace::Clockwise;
        bool DepthClipEnabled = false;
        bool ScissorTestEnabled = false;
        bool CullingEnabled = false;
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
        virtual ~Pipeline() {}
        virtual const PipelineDescription &GetPipelineDescription() const = 0;

    protected:
        PipelineDescription m_Description;

    private:
        friend class GraphicsDevice;
    };
}