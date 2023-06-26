#include "PipelineDX11.h"

#if defined(NX_PLATFORM_DX11)

namespace Nexus
{
    PipelineDX11::PipelineDX11(ID3D11Device *device, const PipelineDescription &description)
        : Pipeline(description)
    {
        m_Device = device;

        SetupDepthStencilState();
        SetupRasterizerState();
    }

    D3D11_COMPARISON_FUNC GetComparisonFunction(ComparisonFunction function)
    {
        switch (function)
        {
        case ComparisonFunction::Always:
            return D3D11_COMPARISON_ALWAYS;
        case ComparisonFunction::Equal:
            return D3D11_COMPARISON_EQUAL;
        case ComparisonFunction::Greater:
            return D3D11_COMPARISON_GREATER;
        case ComparisonFunction::GreaterEqual:
            return D3D11_COMPARISON_GREATER_EQUAL;
        case ComparisonFunction::Less:
            return D3D11_COMPARISON_LESS;
        case ComparisonFunction::LessEqual:
            return D3D11_COMPARISON_LESS_EQUAL;
        case ComparisonFunction::Never:
            return D3D11_COMPARISON_NEVER;
        case ComparisonFunction::NotEqual:
            return D3D11_COMPARISON_NOT_EQUAL;
        }
    }

    D3D11_STENCIL_OP GetStencilOperation(StencilOperation operation)
    {
        switch (operation)
        {
        case StencilOperation::Keep:
            return D3D11_STENCIL_OP_KEEP;
        case StencilOperation::Zero:
            return D3D11_STENCIL_OP_ZERO;
        case StencilOperation::Replace:
            return D3D11_STENCIL_OP_REPLACE;
        case StencilOperation::Increment:
            return D3D11_STENCIL_OP_INCR;
        case StencilOperation::Decrement:
            return D3D11_STENCIL_OP_DECR;
        case StencilOperation::Invert:
            return D3D11_STENCIL_OP_INVERT;
        }
    }

    D3D11_FILL_MODE GetFillMode(FillMode fillMode)
    {
        switch (fillMode)
        {
        case FillMode::Solid:
            return D3D11_FILL_SOLID;
        case FillMode::Wireframe:
            return D3D11_FILL_WIREFRAME;
        }
    }

    D3D11_CULL_MODE GetCullMode(CullMode cullMode)
    {
        switch (cullMode)
        {
        case CullMode::Back:
            return D3D11_CULL_BACK;
        case CullMode::Front:
            return D3D11_CULL_FRONT;
        case CullMode::None:
            return D3D11_CULL_NONE;
        }
    }

    void PipelineDX11::SetupDepthStencilState()
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilDescription;

        // setting up depth testing
        {
            // whether depth testing is enabled
            depthStencilDescription.DepthEnable = m_Description.DepthStencilDescription.EnableDepthTest;

            // whether the depth buffer can be written to during rendering
            if (m_Description.DepthStencilDescription.EnableDepthWrite)
            {
                depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            }
            else
            {
                depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            }

            // how to determine which pixel should be put into depth buffer
            auto depthFunc = GetComparisonFunction(m_Description.DepthStencilDescription.DepthComparisonFunction);
            depthStencilDescription.DepthFunc = depthFunc;
        }

        // setting up stencil state
        {
            depthStencilDescription.StencilEnable = m_Description.DepthStencilDescription.EnableStencilTest;
            depthStencilDescription.StencilReadMask = m_Description.DepthStencilDescription.StencilMask;
            depthStencilDescription.StencilWriteMask = m_Description.DepthStencilDescription.StencilMask;

            auto stencilFailOp = GetStencilOperation(m_Description.DepthStencilDescription.StencilFailOperation);
            auto stencilDepthFailOp = GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthFailOperation);
            auto stencilPassOp = GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthSuccessOperation);
            auto stencilFunc = GetComparisonFunction(m_Description.DepthStencilDescription.StencilComparisonFunction);

            depthStencilDescription.FrontFace.StencilFailOp = stencilFailOp;
            depthStencilDescription.FrontFace.StencilDepthFailOp = stencilDepthFailOp;
            depthStencilDescription.FrontFace.StencilPassOp = stencilPassOp;
            depthStencilDescription.FrontFace.StencilFunc = stencilFunc;

            depthStencilDescription.BackFace.StencilFailOp = stencilFailOp;
            depthStencilDescription.BackFace.StencilDepthFailOp = stencilDepthFailOp;
            depthStencilDescription.BackFace.StencilPassOp = stencilPassOp;
            depthStencilDescription.BackFace.StencilFunc = stencilFunc;
        }

        m_Device->CreateDepthStencilState(&depthStencilDescription, &m_DepthStencilState);
    }

    void PipelineDX11::SetupRasterizerState()
    {
        D3D11_RASTERIZER_DESC rasterizerDesc;

        auto fillMode = GetFillMode(m_Description.RasterizerStateDescription.FillMode);
        auto cullMode = GetCullMode(m_Description.RasterizerStateDescription.CullMode);

        rasterizerDesc.FillMode = fillMode;
        rasterizerDesc.CullMode = cullMode;
        if (m_Description.RasterizerStateDescription.FrontFace == FrontFace::CounterClockwise)
        {
            rasterizerDesc.FrontCounterClockwise = true;
        }
        else
        {
            rasterizerDesc.FrontCounterClockwise = false;
        }

        rasterizerDesc.DepthBias = false;
        rasterizerDesc.DepthBiasClamp = 0;
        rasterizerDesc.SlopeScaledDepthBias = 0;
        rasterizerDesc.DepthClipEnable = m_Description.RasterizerStateDescription.DepthClipEnabled;
        rasterizerDesc.ScissorEnable = m_Description.RasterizerStateDescription.ScissorTestEnabled;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;

        m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterizerState);

        auto &scissorRectangle = m_Description.RasterizerStateDescription.ScissorRectangle;
        D3D11_RECT scissor;
        scissor.left = scissorRectangle.X;
        scissor.right = scissorRectangle.Width;
        scissor.top = scissorRectangle.Y;
        scissor.bottom = scissorRectangle.Height;
        m_ScissorRectangle = scissor;
    }

    D3D11_PRIMITIVE_TOPOLOGY PipelineDX11::GetTopology()
    {
        switch (m_Description.PrimitiveTopology)
        {
        case Topology::LineList:
            return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case Topology::LineStrip:
            return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case Topology::PointList:
            return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case Topology::TriangleList:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case Topology::TriangleStrip:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        }
    }

    const PipelineDescription &PipelineDX11::GetPipelineDescription() const
    {
        return m_Description;
    }
}

#endif