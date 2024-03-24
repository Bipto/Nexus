#include "ShaderModuleD3D12.hpp"

#include "Nexus/Graphics/ShaderGenerator.hpp"

std::string GetShaderVersion(Nexus::Graphics::ShaderStage stage)
{
    switch (stage)
    {
    case Nexus::Graphics::ShaderStage::Fragment:
        return "ps_6_0";
    case Nexus::Graphics::ShaderStage::Geometry:
        return "gs_6_0";
    case Nexus::Graphics::ShaderStage::TesselationControl:
        return "hs_6_0";
    case Nexus::Graphics::ShaderStage::TesselationEvaluation:
        return "ds_6_0";
    case Nexus::Graphics::ShaderStage::Vertex:
        return "vs_6_0";

    default:
        throw std::runtime_error("Failed to find a valid shader stage");
    }
}

namespace Nexus::Graphics
{
    ShaderModuleD3D12::ShaderModuleD3D12(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec)
        : ShaderModule(shaderModuleSpec, resourceSpec)
    {
        Microsoft::WRL::ComPtr<IDxcCompiler3> compiler;
        Microsoft::WRL::ComPtr<IDxcUtils> utils;
        Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
        utils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());

        Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
        utils->CreateBlob(m_ModuleSpecification.Source.c_str(), m_ModuleSpecification.Source.length(), CP_UTF8, sourceBlob.GetAddressOf());

        std::string entryPoint = GetD3DShaderEntryPoint(shaderModuleSpec.Stage);
        std::wstring wsEntryPoint(entryPoint.begin(), entryPoint.end());

        std::string shaderVersion = GetShaderVersion(shaderModuleSpec.Stage);
        std::wstring wsShaderVersion(shaderVersion.begin(), shaderVersion.end());

        std::vector<LPCWSTR> compilationArguments =
            {
                L"-E",
                wsEntryPoint.c_str(),
                L"-T",
                wsShaderVersion.c_str(),
                DXC_ARG_WARNINGS_ARE_ERRORS};

#if defined(DEBUG) || defined(_DEBUG)
        compilationArguments.push_back(DXC_ARG_DEBUG);
#endif

        DxcBuffer sourceBuffer = {};
        sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
        sourceBuffer.Size = sourceBlob->GetBufferSize();
        sourceBuffer.Encoding = 0;

        Microsoft::WRL::ComPtr<IDxcResult> compiledShaderBuffer = {};
        const HRESULT hr = compiler->Compile(
            &sourceBuffer,
            compilationArguments.data(),
            compilationArguments.size(),
            includeHandler.Get(),
            IID_PPV_ARGS(compiledShaderBuffer.GetAddressOf()));

        compiledShaderBuffer->GetResult(&m_ShaderBlob);
    }

    IDxcBlob *ShaderModuleD3D12::GetBlob() const
    {
        return m_ShaderBlob.Get();
    }
}