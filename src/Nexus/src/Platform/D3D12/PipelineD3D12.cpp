#include "PipelineD3D12.hpp"

#if defined(NX_PLATFORM_D3D12)

#include "FramebufferD3D12.hpp"
#include "ShaderModuleD3D12.hpp"
#include "SwapchainD3D12.hpp"

#include "D3D12Utils.hpp"

namespace Nexus::Graphics
{
PipelineD3D12::PipelineD3D12(ID3D12Device10 *device, const PipelineDescription &description) : Pipeline(description), m_Device(device), m_Description(description)
{
    CreateRootSignature();
    CreateInputLayout();
    CreatePipeline();
    CreatePrimitiveTopology();
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

D3D_PRIMITIVE_TOPOLOGY PipelineD3D12::GetD3DPrimitiveTopology()
{
    return m_PrimitiveTopology;
}

void PipelineD3D12::CreateRootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    desc.NumParameters = 0;
    desc.NumStaticSamplers = 0;

    std::vector<D3D12_DESCRIPTOR_RANGE> samplerRanges;
    std::vector<D3D12_DESCRIPTOR_RANGE> textureConstantBufferRanges;

    for (int i = 0; i < m_Description.ResourceSetSpec.SampledImages.size(); i++)
    {
        const auto &textureInfo = m_Description.ResourceSetSpec.SampledImages.at(i);
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(textureInfo.Set, textureInfo.Binding);

        D3D12_DESCRIPTOR_RANGE samplerRange = {};
        samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        samplerRange.BaseShaderRegister = slot;
        samplerRange.NumDescriptors = 1;
        samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        samplerRange.RegisterSpace = 0;
        samplerRanges.push_back(samplerRange);

        D3D12_DESCRIPTOR_RANGE textureRange = {};
        textureRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        textureRange.BaseShaderRegister = slot;
        textureRange.NumDescriptors = 1;
        textureRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        textureRange.RegisterSpace = 0;
        textureConstantBufferRanges.push_back(textureRange);
    }

    for (int i = 0; i < m_Description.ResourceSetSpec.UniformBuffers.size(); i++)
    {
        const auto &uniformBufferInfo = m_Description.ResourceSetSpec.UniformBuffers.at(i);
        uint32_t slot = ResourceSet::GetLinearDescriptorSlot(uniformBufferInfo.Set, uniformBufferInfo.Binding);

        D3D12_DESCRIPTOR_RANGE constantBufferRange = {};
        constantBufferRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        constantBufferRange.BaseShaderRegister = slot;
        constantBufferRange.NumDescriptors = 1;
        constantBufferRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        constantBufferRange.RegisterSpace = 0;
        textureConstantBufferRanges.push_back(constantBufferRange);
    }

    std::vector<D3D12_ROOT_PARAMETER> rootParameters;

    // sampler parameter
    {
        D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
        descriptorTable.NumDescriptorRanges = samplerRanges.size();
        descriptorTable.pDescriptorRanges = samplerRanges.data();

        D3D12_ROOT_PARAMETER parameter = {};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable = descriptorTable;
        parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        if (samplerRanges.size() > 0)
        {
            rootParameters.push_back(parameter);
        }
    }

    // texture constant buffer parameter
    {
        D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
        descriptorTable.NumDescriptorRanges = textureConstantBufferRanges.size();
        descriptorTable.pDescriptorRanges = textureConstantBufferRanges.data();

        D3D12_ROOT_PARAMETER parameter = {};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable = descriptorTable;
        parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        if (textureConstantBufferRanges.size() > 0)
        {
            rootParameters.push_back(parameter);
        }
    }

    desc.NumParameters = rootParameters.size();
    desc.pParameters = rootParameters.data();

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
    uint32_t sampleCount = GetSampleCount(m_Description.ColourTargetSampleCount);
    std::vector<DXGI_FORMAT> rtvFormats;

    for (uint32_t index = 0; index < m_Description.ColourTargetCount; index++)
    {
        DXGI_FORMAT colourFormat = D3D12::GetD3D12PixelFormat(m_Description.ColourFormats.at(index), false);
        rtvFormats.push_back(colourFormat);
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
    pipelineDesc.pRootSignature = m_RootSignature.Get();
    pipelineDesc.InputLayout.NumElements = m_InputLayout.size();
    pipelineDesc.InputLayout.pInputElementDescs = m_InputLayout.data();
    pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

    // setup shaders
    pipelineDesc.VS.BytecodeLength = 0;
    pipelineDesc.VS.pShaderBytecode = nullptr;
    pipelineDesc.PS.BytecodeLength = 0;
    pipelineDesc.PS.pShaderBytecode = nullptr;
    pipelineDesc.DS.BytecodeLength = 0;
    pipelineDesc.DS.pShaderBytecode = nullptr;
    pipelineDesc.HS.BytecodeLength = 0;
    pipelineDesc.HS.pShaderBytecode = nullptr;
    pipelineDesc.GS.BytecodeLength = 0;
    pipelineDesc.GS.pShaderBytecode = nullptr;

    if (m_Description.FragmentModule)
    {
        auto d3d12FragmentModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.FragmentModule);
        NX_ASSERT(d3d12FragmentModule->GetShaderStage() == ShaderStage::Fragment, "Shader module is not a fragment shader");
        auto blob = d3d12FragmentModule->GetBlob();

        pipelineDesc.PS.BytecodeLength = blob->GetBufferSize();
        pipelineDesc.PS.pShaderBytecode = blob->GetBufferPointer();
    }

    if (m_Description.GeometryModule)
    {
        auto d3d12GeometryModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.GeometryModule);
        NX_ASSERT(d3d12GeometryModule->GetShaderStage() == ShaderStage::Geometry, "Shader module is not a geometry shader");
        auto blob = d3d12GeometryModule->GetBlob();

        pipelineDesc.GS.BytecodeLength = blob->GetBufferSize();
        pipelineDesc.GS.pShaderBytecode = blob->GetBufferPointer();
    }

    if (m_Description.TesselationControlModule)
    {
        auto d3d12TesselationControlModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.TesselationControlModule);
        NX_ASSERT(d3d12TesselationControlModule->GetShaderStage() == ShaderStage::TesselationControl, "Shader module is not a tesselation control shader");
        auto blob = d3d12TesselationControlModule->GetBlob();

        pipelineDesc.HS.BytecodeLength = blob->GetBufferSize();
        pipelineDesc.HS.pShaderBytecode = blob->GetBufferPointer();
    }

    if (m_Description.TesselationEvaluationModule)
    {
        auto d3d12TesselationEvaluationModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.TesselationEvaluationModule);
        NX_ASSERT(d3d12TesselationEvaluationModule->GetShaderStage() == ShaderStage::TesselationEvaluation, "Shader module is not a tesselation evaluation shader");
        auto blob = d3d12TesselationEvaluationModule->GetBlob();

        pipelineDesc.DS.BytecodeLength = blob->GetBufferSize();
        pipelineDesc.DS.pShaderBytecode = blob->GetBufferPointer();
    }

    if (m_Description.VertexModule)
    {
        auto d3d12VertexModule = std::dynamic_pointer_cast<ShaderModuleD3D12>(m_Description.VertexModule);
        NX_ASSERT(d3d12VertexModule->GetShaderStage() == ShaderStage::Vertex, "Shader module is not a vertex shader");
        auto blob = d3d12VertexModule->GetBlob();

        pipelineDesc.VS.BytecodeLength = blob->GetBufferSize();
        pipelineDesc.VS.pShaderBytecode = blob->GetBufferPointer();
    }

    pipelineDesc.PrimitiveTopologyType = GetPipelinePrimitiveTopologyType();
    pipelineDesc.RasterizerState = CreateRasterizerState();
    pipelineDesc.StreamOutput = CreateStreamOutputDesc();
    pipelineDesc.NumRenderTargets = rtvFormats.size();

    for (uint32_t rtvIndex = 0; rtvIndex < rtvFormats.size(); rtvIndex++)
    {
        pipelineDesc.RTVFormats[rtvIndex] = rtvFormats.at(rtvIndex);
    }

    DXGI_FORMAT depthFormat = D3D12::GetD3D12PixelFormat(m_Description.DepthFormat, true);
    pipelineDesc.DSVFormat = depthFormat;
    pipelineDesc.BlendState = CreateBlendStateDesc();
    pipelineDesc.DepthStencilState = CreateDepthStencilDesc();
    pipelineDesc.SampleMask = 0xFFFFFFFF;
    pipelineDesc.SampleDesc.Count = sampleCount;
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

    uint32_t elementIndex = 0;
    for (uint32_t layoutIndex = 0; layoutIndex < m_Description.Layouts.size(); layoutIndex++)
    {
        const auto &layout = m_Description.Layouts.at(layoutIndex);

        for (uint32_t i = 0; i < layout.GetNumberOfElements(); i++)
        {
            const auto &element = layout.GetElement(i);

            D3D12_INPUT_CLASSIFICATION classification =
                (layout.GetInstanceStepRate() != 0) ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

            D3D12_INPUT_ELEMENT_DESC desc = {element.Name.c_str(), elementIndex,   D3D12::GetD3D12BaseType(element), layoutIndex,
                                             element.Offset,       classification, layout.GetInstanceStepRate()};

            m_InputLayout.push_back(desc);
            elementIndex++;
        }
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
    desc.CullMode = D3D12::GetCullMode(m_Description.RasterizerStateDesc.TriangleCullMode);

    if (m_Description.RasterizerStateDesc.TriangleFrontFace == Nexus::Graphics::FrontFace::CounterClockwise)
    {
        desc.FrontCounterClockwise = true;
    }
    else
    {
        desc.FrontCounterClockwise = false;
    }

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
    desc.RenderTarget[0].BlendEnable = m_Description.BlendStateDesc.EnableBlending;
    desc.RenderTarget[0].SrcBlend = D3D12::GetBlendFunction(m_Description.BlendStateDesc.SourceColourBlend);
    desc.RenderTarget[0].DestBlend = D3D12::GetBlendFunction(m_Description.BlendStateDesc.DestinationColourBlend);
    desc.RenderTarget[0].BlendOp = D3D12::GetBlendEquation(m_Description.BlendStateDesc.ColorBlendFunction);
    desc.RenderTarget[0].SrcBlendAlpha = D3D12::GetBlendFunction(m_Description.BlendStateDesc.SourceAlphaBlend);
    desc.RenderTarget[0].DestBlendAlpha = D3D12::GetBlendFunction(m_Description.BlendStateDesc.DestinationAlphaBlend);
    desc.RenderTarget[0].BlendOpAlpha = D3D12::GetBlendEquation(m_Description.BlendStateDesc.AlphaBlendFunction);
    desc.RenderTarget[0].LogicOpEnable = FALSE;
    desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    return desc;
}

D3D12_DEPTH_STENCIL_DESC PipelineD3D12::CreateDepthStencilDesc()
{
    D3D12_DEPTH_STENCIL_DESC desc{};
    desc.DepthEnable = m_Description.DepthStencilDesc.EnableDepthTest;
    desc.DepthFunc = D3D12::GetComparisonFunction(m_Description.DepthStencilDesc.DepthComparisonFunction);

    if (m_Description.DepthStencilDesc.EnableDepthWrite)
    {
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    }
    else
    {
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    }

    desc.StencilEnable = m_Description.DepthStencilDesc.EnableStencilTest;
    desc.StencilReadMask = m_Description.DepthStencilDesc.StencilMask;
    desc.StencilWriteMask = m_Description.DepthStencilDesc.StencilMask;

    auto stencilFailOp = D3D12::GetStencilOperation(m_Description.DepthStencilDesc.StencilFailOperation);
    auto stencilDepthFailOp = D3D12::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthFailOperation);
    auto stencilPassOp = D3D12::GetStencilOperation(m_Description.DepthStencilDesc.StencilSuccessDepthSuccessOperation);
    auto stencilFunc = D3D12::GetComparisonFunction(m_Description.DepthStencilDesc.StencilComparisonFunction);

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

D3D12_PRIMITIVE_TOPOLOGY_TYPE PipelineD3D12::GetPipelinePrimitiveTopologyType()
{
    switch (m_Description.PrimitiveTopology)
    {
    case Topology::LineList:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        break;
    case Topology::LineStrip:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        break;
    case Topology::PointList:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        break;
    case Topology::TriangleList:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        break;
    case Topology::TriangleStrip:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        break;
    }
}
} // namespace Nexus::Graphics

#endif