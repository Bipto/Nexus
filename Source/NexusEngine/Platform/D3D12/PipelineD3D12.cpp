#include "PipelineD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

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

    D3D12_CULL_MODE GetCullMode(CullMode cullMode)
    {
        switch (cullMode)
        {
        case CullMode::Back:
            return D3D12_CULL_MODE_BACK;
        case CullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        case CullMode::None:
            return D3D12_CULL_MODE_NONE;
        }
    }

    D3D12_COMPARISON_FUNC GetComparisonFunction(ComparisonFunction function)
    {
        switch (function)
        {
        case ComparisonFunction::Always:
            return D3D12_COMPARISON_FUNC_ALWAYS;
        case ComparisonFunction::Equal:
            return D3D12_COMPARISON_FUNC_EQUAL;
        case ComparisonFunction::Greater:
            return D3D12_COMPARISON_FUNC_GREATER;
        case ComparisonFunction::GreaterEqual:
            return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case ComparisonFunction::Less:
            return D3D12_COMPARISON_FUNC_LESS;
        case ComparisonFunction::LessEqual:
            return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case ComparisonFunction::Never:
            return D3D12_COMPARISON_FUNC_NEVER;
        case ComparisonFunction::NotEqual:
            return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        }
    }

    D3D12_STENCIL_OP GetStencilOperation(StencilOperation operation)
    {
        switch (operation)
        {
        case StencilOperation::Keep:
            return D3D12_STENCIL_OP_KEEP;
        case StencilOperation::Zero:
            return D3D12_STENCIL_OP_ZERO;
        case StencilOperation::Replace:
            return D3D12_STENCIL_OP_REPLACE;
        case StencilOperation::Increment:
            return D3D12_STENCIL_OP_INCR;
        case StencilOperation::Decrement:
            return D3D12_STENCIL_OP_DECR;
        case StencilOperation::Invert:
            return D3D12_STENCIL_OP_INVERT;
        }
    }

    D3D12_FILL_MODE GetFillMode(FillMode fillMode)
    {
        switch (fillMode)
        {
        case FillMode::Solid:
            return D3D12_FILL_MODE_SOLID;
        case FillMode::Wireframe:
            return D3D12_FILL_MODE_WIREFRAME;
        }
    }

    D3D12_BLEND GetBlendFunction(BlendFunction function)
    {
        switch (function)
        {
        case BlendFunction::Zero:
            return D3D12_BLEND_ZERO;
        case BlendFunction::One:
            return D3D12_BLEND_ONE;
        case BlendFunction::SourceColor:
            return D3D12_BLEND_SRC_COLOR;
        case BlendFunction::OneMinusSourceColor:
            return D3D12_BLEND_INV_SRC_COLOR;
        case BlendFunction::DestinationColor:
            return D3D12_BLEND_DEST_COLOR;
        case BlendFunction::OneMinusDestinationColor:
            return D3D12_BLEND_INV_DEST_COLOR;
        case BlendFunction::SourceAlpha:
            return D3D12_BLEND_SRC_ALPHA;
        case BlendFunction::OneMinusSourceAlpha:
            return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFunction::DestinationAlpha:
            return D3D12_BLEND_DEST_ALPHA;
        case BlendFunction::OneMinusDestinationAlpha:
            return D3D12_BLEND_INV_DEST_ALPHA;
        }
    }

    D3D12_BLEND_OP GetBlendEquation(BlendEquation equation)
    {
        switch (equation)
        {
        case BlendEquation::Add:
            return D3D12_BLEND_OP_ADD;
        case BlendEquation::Subtract:
            return D3D12_BLEND_OP_SUBTRACT;
        case BlendEquation::ReverseSubtract:
            return D3D12_BLEND_OP_REV_SUBTRACT;
        case BlendEquation::Min:
            return D3D12_BLEND_OP_MIN;
        case BlendEquation::Max:
            return D3D12_BLEND_OP_MAX;
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

        std::vector<D3D12_ROOT_PARAMETER> parameters;

        D3D12_DESCRIPTOR_RANGE samplerRange;
        samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        samplerRange.BaseShaderRegister = 0;
        samplerRange.NumDescriptors = m_Description.ResourceSetSpecification.TextureBindings.size();
        samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        samplerRange.RegisterSpace = 0;

        D3D12_ROOT_DESCRIPTOR_TABLE samplerTable;
        samplerTable.NumDescriptorRanges = 1;
        samplerTable.pDescriptorRanges = &samplerRange;

        D3D12_DESCRIPTOR_RANGE textureRange;
        textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        textureRange.BaseShaderRegister = 0;
        textureRange.NumDescriptors = m_Description.ResourceSetSpecification.TextureBindings.size();
        textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        textureRange.RegisterSpace = 0;

        D3D12_ROOT_DESCRIPTOR_TABLE textureTable;
        textureTable.NumDescriptorRanges = 1;
        textureTable.pDescriptorRanges = &textureRange;

        D3D12_DESCRIPTOR_RANGE constantBufferRange;
        constantBufferRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        constantBufferRange.BaseShaderRegister = 0;
        constantBufferRange.NumDescriptors = m_Description.ResourceSetSpecification.UniformResourceBindings.size();
        constantBufferRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        constantBufferRange.RegisterSpace = 0;

        D3D12_ROOT_DESCRIPTOR_TABLE constantBufferTable;
        constantBufferTable.NumDescriptorRanges = 1;
        constantBufferTable.pDescriptorRanges = &constantBufferRange;

        // setup samplers and textures
        for (const auto &textureBinding : m_Description.ResourceSetSpecification.TextureBindings)
        {
            D3D12_ROOT_PARAMETER samplerParameter;
            samplerParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            samplerParameter.DescriptorTable = samplerTable;
            samplerParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
            parameters.push_back(samplerParameter);

            D3D12_ROOT_PARAMETER textureParameter;
            textureParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            textureParameter.DescriptorTable = textureTable;
            textureParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
            parameters.push_back(textureParameter);
        }

        // setup constant buffers
        for (const auto &uniformBufferBinding : m_Description.ResourceSetSpecification.UniformResourceBindings)
        {
            D3D12_ROOT_PARAMETER constantBufferParameter;
            constantBufferParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            constantBufferParameter.DescriptorTable = constantBufferTable;
            constantBufferParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
            parameters.push_back(constantBufferParameter);
        }

        desc.NumParameters = parameters.size();
        desc.pParameters = parameters.data();

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
            break;
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
        desc.CullMode = GetCullMode(m_Description.RasterizerStateDescription.CullMode);
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
        desc.RenderTarget[0].SrcBlend = GetBlendFunction(m_Description.BlendStateDescription.SourceColourBlend);
        desc.RenderTarget[0].DestBlend = GetBlendFunction(m_Description.BlendStateDescription.DestinationColourBlend);
        desc.RenderTarget[0].BlendOp = GetBlendEquation(m_Description.BlendStateDescription.BlendEquation);
        desc.RenderTarget[0].SrcBlendAlpha = GetBlendFunction(m_Description.BlendStateDescription.SourceAlphaBlend);
        desc.RenderTarget[0].DestBlendAlpha = GetBlendFunction(m_Description.BlendStateDescription.DestinationAlphaBlend);
        desc.RenderTarget[0].BlendOpAlpha = GetBlendEquation(m_Description.BlendStateDescription.BlendEquation);
        desc.RenderTarget[0].LogicOpEnable = FALSE;
        desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        return desc;
    }

    D3D12_DEPTH_STENCIL_DESC PipelineD3D12::CreateDepthStencilDesc()
    {
        D3D12_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = m_Description.DepthStencilDescription.EnableDepthTest;
        desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        if (m_Description.DepthStencilDescription.EnableDepthWrite)
        {
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        }
        else
        {
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        }

        desc.StencilEnable = m_Description.DepthStencilDescription.EnableStencilTest;
        desc.StencilReadMask = m_Description.DepthStencilDescription.StencilMask;
        desc.StencilWriteMask = m_Description.DepthStencilDescription.StencilMask;

        auto stencilFailOp = GetStencilOperation(m_Description.DepthStencilDescription.StencilFailOperation);
        auto stencilDepthFailOp = GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthFailOperation);
        auto stencilPassOp = GetStencilOperation(m_Description.DepthStencilDescription.StencilSuccessDepthSuccessOperation);
        auto stencilFunc = GetComparisonFunction(m_Description.DepthStencilDescription.StencilComparisonFunction);

        desc.FrontFace.StencilFunc = stencilFunc;
        desc.FrontFace.StencilDepthFailOp = stencilDepthFailOp;
        desc.FrontFace.StencilFailOp = stencilFailOp;
        desc.FrontFace.StencilPassOp = stencilPassOp;
        desc.BackFace.StencilFunc = stencilFunc;
        desc.BackFace.StencilDepthFailOp = stencilDepthFailOp;
        desc.BackFace.StencilFailOp = stencilFailOp;
        desc.BackFace.StencilPassOp = stencilPassOp;
        return desc;
    }
}

#endif