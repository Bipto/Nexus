#include "PipelineD3D12.hpp"

#include "ShaderD3D12.hpp"

namespace Nexus::Graphics
{
    DXGI_FORMAT GetD3D12BaseType(const VertexBufferElement element)
    {
        switch (element.Type)
        {
        case ShaderDataType::Float:
            return DXGI_FORMAT_R32_FLOAT;
        case ShaderDataType::Float2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case ShaderDataType::Float3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case ShaderDataType::Float4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ShaderDataType::Int:
            return DXGI_FORMAT_R32_SINT;
        case ShaderDataType::Int2:
            return DXGI_FORMAT_R32G32_SINT;
        case ShaderDataType::Int3:
            return DXGI_FORMAT_R32G32B32_SINT;
        case ShaderDataType::Int4:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        }
    }

    PipelineD3D12::PipelineD3D12(ID3D12Device10 *device, const PipelineDescription &description)
        : Pipeline(description), m_Device(device), m_Description(description)
    {
        CreateRootSignature();
        CreateInputLayout();
        CreatePipeline();
        CreatePrimitiveTopology();

        m_Viewport.TopLeftX = description.Viewport.X;
        m_Viewport.TopLeftY = description.Viewport.Y;
        m_Viewport.Width = description.Viewport.Width;
        m_Viewport.Height = description.Viewport.Height;
        m_Viewport.MinDepth = description.Viewport.MinDepth;
        m_Viewport.MaxDepth = description.Viewport.MaxDepth;

        m_ScissorRectangle.left = description.RasterizerStateDescription.ScissorRectangle.X;
        m_ScissorRectangle.top = description.RasterizerStateDescription.ScissorRectangle.Y;
        m_ScissorRectangle.right = description.RasterizerStateDescription.ScissorRectangle.Width;
        m_ScissorRectangle.bottom = description.RasterizerStateDescription.ScissorRectangle.Height;
    }

    PipelineD3D12::~PipelineD3D12()
    {
    }

    const PipelineDescription &PipelineD3D12::GetPipelineDescription() const
    {
        return m_Description;
    }

    ID3D12RootSignature *PipelineD3D12::GetRootSignature()
    {
        return m_RootSignature.Get();
    }

    ID3D12PipelineState *PipelineD3D12::GetPipelineState()
    {
        return m_PipelineStateObject.Get();
    }

    D3D_PRIMITIVE_TOPOLOGY PipelineD3D12::GetPrimitiveTopology()
    {
        return m_PrimitiveTopology;
    }

    const D3D12_VIEWPORT &PipelineD3D12::GetViewport()
    {
        return m_Viewport;
    }

    const RECT &PipelineD3D12::GetScissorRectangle()
    {
        return m_ScissorRectangle;
    }

    void PipelineD3D12::CreateRootSignature()
    {
        D3D12_ROOT_SIGNATURE_DESC desc;
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        desc.NumParameters = 0;
        desc.NumStaticSamplers = 0;

        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        if (SUCCEEDED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &m_RootSignatureBlob, &errorBlob)))
        {
            m_Device->CreateRootSignature(0, m_RootSignatureBlob->GetBufferPointer(), m_RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
        }
        else
        {
            std::string errorMessage = std::string((char *)errorBlob->GetBufferPointer());
            std::cout << errorMessage << std::endl;
        }
    }

    void PipelineD3D12::CreatePipeline()
    {
        auto shaderD3D12 = (ShaderD3D12 *)m_Description.Shader;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
        pipelineDesc.pRootSignature = m_RootSignature.Get();
        pipelineDesc.InputLayout.NumElements = m_InputLayout.size();
        pipelineDesc.InputLayout.pInputElementDescs = m_InputLayout.data();
        pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
        pipelineDesc.VS.BytecodeLength = shaderD3D12->GetVertexShaderBlob()->GetBufferSize();
        pipelineDesc.VS.pShaderBytecode = shaderD3D12->GetVertexShaderBlob()->GetBufferPointer();
        pipelineDesc.PS.BytecodeLength = shaderD3D12->GetFragmentShaderBlob()->GetBufferSize();
        pipelineDesc.PS.pShaderBytecode = shaderD3D12->GetFragmentShaderBlob()->GetBufferPointer();
        pipelineDesc.DS.BytecodeLength = 0;
        pipelineDesc.DS.pShaderBytecode = nullptr;
        pipelineDesc.HS.BytecodeLength = 0;
        pipelineDesc.HS.pShaderBytecode = nullptr;
        pipelineDesc.GS.BytecodeLength = 0;
        pipelineDesc.GS.pShaderBytecode = nullptr;
        pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipelineDesc.RasterizerState = CreateRasterizerState();
        pipelineDesc.StreamOutput = CreateStreamOutputDesc();
        pipelineDesc.NumRenderTargets = 1;
        pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        pipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
        pipelineDesc.BlendState = CreateBlendStateDesc();
        pipelineDesc.DepthStencilState = CreateDepthStencilDesc();
        pipelineDesc.SampleMask = 0xFFFFFFFF;
        pipelineDesc.SampleDesc.Count = 1;
        pipelineDesc.SampleDesc.Quality = 0;
        pipelineDesc.NodeMask = 0;
        pipelineDesc.CachedPSO.CachedBlobSizeInBytes = 0;
        pipelineDesc.CachedPSO.pCachedBlob = nullptr;
        pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

        HRESULT hr = m_Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_PipelineStateObject));
        if (FAILED(hr))
        {
            _com_error error(hr);
            std::cout << "Failed to create pipeline state: " << error.ErrorMessage() << std::endl;
        }
    }

    void PipelineD3D12::CreateInputLayout()
    {
        m_InputLayout.clear();
        auto shader = m_Description.Shader;
        const auto &bufferLayout = shader->GetLayout();

        unsigned int index = 0;
        for (auto &element : bufferLayout)
        {
            D3D12_INPUT_ELEMENT_DESC desc =
                {
                    element.Name.c_str(),
                    index,
                    GetD3D12BaseType(element),
                    0,
                    element.Offset,
                    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                    0};
            m_InputLayout.push_back(desc);
            index++;
        }
    }

    void PipelineD3D12::CreatePrimitiveTopology()
    {
        switch (m_Description.PrimitiveTopology)
        {
        case Topology::LineList:
            m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case Topology::LineStrip:
            m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case Topology::PointList:
            m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case Topology::TriangleList:
            m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case Topology::TriangleStrip:
            m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        }
    }

    D3D12_RASTERIZER_DESC PipelineD3D12::CreateRasterizerState()
    {
        D3D12_RASTERIZER_DESC desc{};
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12_CULL_MODE_NONE;
        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = .0f;
        desc.SlopeScaledDepthBias = .0f;
        desc.DepthClipEnable = FALSE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        desc.ForcedSampleCount = 0;
        return desc;
    }

    D3D12_STREAM_OUTPUT_DESC PipelineD3D12::CreateStreamOutputDesc()
    {
        D3D12_STREAM_OUTPUT_DESC desc{};
        desc.NumEntries = 0;
        desc.NumStrides = 0;
        desc.pBufferStrides = nullptr;
        desc.RasterizedStream = 0;
        return desc;
    }

    D3D12_BLEND_DESC PipelineD3D12::CreateBlendStateDesc()
    {
        D3D12_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = TRUE;
        desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
        desc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
        desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
        desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        desc.RenderTarget[0].LogicOpEnable = FALSE;
        desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        return desc;
    }

    D3D12_DEPTH_STENCIL_DESC PipelineD3D12::CreateDepthStencilDesc()
    {
        D3D12_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = FALSE;
        desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        desc.StencilEnable = FALSE;
        desc.StencilReadMask = 0;
        desc.StencilWriteMask = 0;
        desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        return desc;
    }
}