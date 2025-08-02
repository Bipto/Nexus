#if defined(NX_PLATFORM_D3D12)

	#include "ShaderModuleD3D12.hpp"

	#include "Nexus-Core/Graphics/ShaderGenerator.hpp"

	#include <dxcapi.h>
	#include <d3d12shader.h>

std::string GetShaderVersion(Nexus::Graphics::ShaderStage stage)
{
	switch (stage)
	{
		case Nexus::Graphics::ShaderStage::Fragment: return "ps_6_0";
		case Nexus::Graphics::ShaderStage::Geometry: return "gs_6_0";
		case Nexus::Graphics::ShaderStage::TesselationControl: return "hs_6_0";
		case Nexus::Graphics::ShaderStage::TesselationEvaluation: return "ds_6_0";
		case Nexus::Graphics::ShaderStage::Vertex: return "vs_6_0";
		case Nexus::Graphics::ShaderStage::Compute: return "cs_6_0";
		case Nexus::Graphics::ShaderStage::Task: return "ts_6_0";
		case Nexus::Graphics::ShaderStage::Mesh: return "ms_6_0";

		default: throw std::runtime_error("Failed to find a valid shader stage");
	}
}

namespace Nexus::Graphics
{
	UINT GetComponentCount(BYTE mask)
	{
		UINT count = 0;
		for (UINT i = 0; i < 4; ++i)
		{
			if (mask & (1 << i))
				++count;
		}
		return count;
	}

	ShaderModuleD3D12::ShaderModuleD3D12(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec)
		: ShaderModule(shaderModuleSpec, resourceSpec)
	{
		Microsoft::WRL::ComPtr<IDxcCompiler3>	   compiler;
		Microsoft::WRL::ComPtr<IDxcUtils>		   utils;
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		utils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());

		Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
		utils->CreateBlob(m_ModuleSpecification.Source.c_str(), m_ModuleSpecification.Source.length(), CP_UTF8, sourceBlob.GetAddressOf());

		std::string	 entryPoint = GetD3DShaderEntryPoint(shaderModuleSpec.ShadingStage);
		std::wstring wsEntryPoint(entryPoint.begin(), entryPoint.end());

		std::string	 shaderVersion = GetShaderVersion(shaderModuleSpec.ShadingStage);
		std::wstring wsShaderVersion(shaderVersion.begin(), shaderVersion.end());

		std::vector<LPCWSTR> compilationArguments =
			{L"-E", wsEntryPoint.c_str(), L"-T", wsShaderVersion.c_str(), DXC_ARG_WARNINGS_ARE_ERRORS, L"-Qstrip_reflect"};

	#if defined(DEBUG) || defined(_DEBUG)
		compilationArguments.push_back(DXC_ARG_DEBUG);
	#endif

		DxcBuffer sourceBuffer = {};
		sourceBuffer.Ptr	   = sourceBlob->GetBufferPointer();
		sourceBuffer.Size	   = sourceBlob->GetBufferSize();
		sourceBuffer.Encoding  = 0;

		Microsoft::WRL::ComPtr<IDxcResult> compiledShaderBuffer = {};
		const HRESULT					   hr					= compiler->Compile(&sourceBuffer,
												compilationArguments.data(),
												compilationArguments.size(),
												includeHandler.Get(),
												IID_PPV_ARGS(compiledShaderBuffer.GetAddressOf()));

		compiledShaderBuffer->GetResult(&m_ShaderBlob);

		ReflectShader(utils, compiledShaderBuffer);
	}

	Microsoft::WRL::ComPtr<IDxcBlob> ShaderModuleD3D12::GetBlob() const
	{
		return m_ShaderBlob;
	}

	ShaderReflectionData ShaderModuleD3D12::Reflect() const
	{
		ShaderReflectionData data;
		return data;
	}

	void ShaderModuleD3D12::ReflectShader(Microsoft::WRL::ComPtr<IDxcUtils> utils, Microsoft::WRL::ComPtr<IDxcResult> compileResult)
	{
		m_ReflectionData.Inputs.clear();

		Microsoft::WRL::ComPtr<IDxcBlob> reflectionData;
		compileResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(reflectionData.GetAddressOf()), nullptr);

		DxcBuffer reflectionBuffer;
		reflectionBuffer.Ptr	  = reflectionData->GetBufferPointer();
		reflectionBuffer.Size	  = reflectionData->GetBufferSize();
		reflectionBuffer.Encoding = 0;

		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shaderReflection;
		utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(shaderReflection.GetAddressOf()));

		D3D12_SHADER_DESC shaderDesc;
		shaderReflection->GetDesc(&shaderDesc);

		for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC input;
			shaderReflection->GetInputParameterDesc(i, &input);

			std::string					name		   = input.SemanticName ? input.SemanticName : "";
			std::string					fullName	   = name + std::to_string(input.SemanticIndex);
			UINT						index		   = input.SemanticIndex;
			D3D_REGISTER_COMPONENT_TYPE type		   = input.ComponentType;
			UINT						inputRegister  = input.Register;
			D3D_NAME					valueType	   = input.SystemValueType;
			UINT						componentCount = GetComponentCount(input.Mask);

			Nexus::Graphics::Attribute &attribute = m_ReflectionData.Inputs.emplace_back();
			attribute.Binding					  = inputRegister;
			attribute.Name						  = fullName;
		}

		for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			shaderReflection->GetResourceBindingDesc(i, &bindDesc);

			std::string			  name		= bindDesc.Name;
			D3D_SHADER_INPUT_TYPE type		= bindDesc.Type;
			D3D_SRV_DIMENSION	  dimension = bindDesc.Dimension;
		}
	}
}	 // namespace Nexus::Graphics

#endif