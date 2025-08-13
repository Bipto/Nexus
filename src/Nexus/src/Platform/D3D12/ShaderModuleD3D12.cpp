#if defined(NX_PLATFORM_D3D12)

	#include "ShaderModuleD3D12.hpp"

	#include "Nexus-Core/Graphics/ShaderGenerator.hpp"

	#include <dxcapi.h>
	#include <dxc/dxcapi.h>
	#include <d3d12shader.h>

std::string GetShaderVersion(Nexus::Graphics::ShaderStage stage)
{
	switch (stage)
	{
		case Nexus::Graphics::ShaderStage::Fragment: return "ps_6_0";
		case Nexus::Graphics::ShaderStage::Geometry: return "gs_6_0";
		case Nexus::Graphics::ShaderStage::TessellationControl: return "hs_6_0";
		case Nexus::Graphics::ShaderStage::TessellationEvaluation: return "ds_6_0";
		case Nexus::Graphics::ShaderStage::Vertex: return "vs_6_0";
		case Nexus::Graphics::ShaderStage::Compute: return "cs_6_0";
		case Nexus::Graphics::ShaderStage::Task: return "ts_6_5";
		case Nexus::Graphics::ShaderStage::Mesh: return "ms_6_5";

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

	ShaderModuleD3D12::ShaderModuleD3D12(const ShaderModuleSpecification &shaderModuleSpec) : ShaderModule(shaderModuleSpec)
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

		std::vector<LPCWSTR> compilationArguments = {L"-E", wsEntryPoint.c_str(), L"-T", wsShaderVersion.c_str(), DXC_ARG_WARNINGS_ARE_ERRORS};

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

		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create shader module");
		}

		compiledShaderBuffer->GetResult(&m_ShaderBlob);

		ReflectShader(utils, compiledShaderBuffer);
	}

	Microsoft::WRL::ComPtr<IDxcBlob> ShaderModuleD3D12::GetBlob() const
	{
		return m_ShaderBlob;
	}

	ShaderReflectionData ShaderModuleD3D12::Reflect() const
	{
		return m_ReflectionData;
	}

	ReflectedShaderDataType ExtractComponentType(D3D_REGISTER_COMPONENT_TYPE componentType, UINT componentCount)
	{
		switch (componentType)
		{
			case D3D_REGISTER_COMPONENT_UINT32:
			{
				switch (componentCount)
				{
					case 1: return ReflectedShaderDataType::UInt;
					case 2: return ReflectedShaderDataType::UInt2;
					case 3: return ReflectedShaderDataType::UInt3;
					case 4: return ReflectedShaderDataType::UInt4;
					default: throw std::runtime_error("Failed to find a valid type");
				}
			}
			case D3D_REGISTER_COMPONENT_SINT32:
			{
				switch (componentCount)
				{
					case 1: return ReflectedShaderDataType::Int;
					case 2: return ReflectedShaderDataType::Int2;
					case 3: return ReflectedShaderDataType::Int3;
					case 4: return ReflectedShaderDataType::Int4;
					default: throw std::runtime_error("Failed to find a valid type");
				}
			}
			case D3D_REGISTER_COMPONENT_FLOAT32:
			{
				switch (componentCount)
				{
					case 1: return ReflectedShaderDataType::Float;
					case 2: return ReflectedShaderDataType::Float2;
					case 3: return ReflectedShaderDataType::Float3;
					case 4: return ReflectedShaderDataType::Float4;
					default: throw std::runtime_error("Failed to find a valid type");
				}
			}
			default: throw std::runtime_error("Failed to find valid register component type");
		}
	}

	void ExtractAttribute(std::vector<Attribute> &attributes, D3D12_SIGNATURE_PARAMETER_DESC shaderParameter)
	{
		std::string					name		   = shaderParameter.SemanticName ? shaderParameter.SemanticName : "";
		std::string					fullName	   = name + std::to_string(shaderParameter.SemanticIndex);
		UINT						index		   = shaderParameter.SemanticIndex;
		D3D_REGISTER_COMPONENT_TYPE type		   = shaderParameter.ComponentType;
		UINT						inputRegister  = shaderParameter.Register;
		D3D_NAME					valueType	   = shaderParameter.SystemValueType;
		UINT						componentCount = GetComponentCount(shaderParameter.Mask);
		UINT						streamIndex	   = shaderParameter.Stream;

		Nexus::Graphics::Attribute &attribute = attributes.emplace_back();
		attribute.Binding					  = inputRegister;
		attribute.Name						  = fullName;
		attribute.Type						  = ExtractComponentType(type, componentCount);
		attribute.StreamIndex				  = streamIndex;
	}

	std::pair<ReflectedShaderDataType, StorageResourceAccess> ExtractShaderInputType(D3D_SHADER_INPUT_TYPE type, UINT flags)
	{
		switch (type)
		{
			case D3D_SIT_CBUFFER: return {ReflectedShaderDataType::UniformBuffer, StorageResourceAccess::None};
			case D3D_SIT_TBUFFER: return {ReflectedShaderDataType::UniformTextureBuffer, StorageResourceAccess::Read};
			case D3D_SIT_TEXTURE: return {ReflectedShaderDataType::Texture, StorageResourceAccess::None};
			case D3D_SIT_SAMPLER:
			{
				if (flags & D3D_SIF_COMPARISON_SAMPLER)
				{
					return {ReflectedShaderDataType::ComparisonSampler, StorageResourceAccess::None};
				}
				else
				{
					return {ReflectedShaderDataType::Sampler, StorageResourceAccess::None};
				}
			}
			case D3D_SIT_UAV_RWTYPED: return {ReflectedShaderDataType::StorageTextureBuffer, StorageResourceAccess::ReadWrite};
			case D3D_SIT_STRUCTURED: return {ReflectedShaderDataType::StorageBuffer, StorageResourceAccess::Read};
			case D3D_SIT_UAV_RWSTRUCTURED: return {ReflectedShaderDataType::StorageBuffer, StorageResourceAccess::ReadWrite};
			case D3D_SIT_BYTEADDRESS: return {ReflectedShaderDataType::StorageBuffer, StorageResourceAccess::ReadByteAddress};
			case D3D_SIT_UAV_RWBYTEADDRESS: return {ReflectedShaderDataType::StorageBuffer, StorageResourceAccess::ReadWriteByteAddress};
			case D3D_SIT_UAV_APPEND_STRUCTURED: return {ReflectedShaderDataType::StorageBuffer, StorageResourceAccess::AppendStructured};
			case D3D_SIT_UAV_CONSUME_STRUCTURED: return {ReflectedShaderDataType::StorageBuffer, StorageResourceAccess::ConsumeStructured};
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				return {ReflectedShaderDataType::StorageBuffer, StorageResourceAccess::ReadWriteStructuredWithCounter};
			case D3D_SIT_RTACCELERATIONSTRUCTURE: return {ReflectedShaderDataType::AccelerationStructure, StorageResourceAccess::None};
			case D3D_SIT_UAV_FEEDBACKTEXTURE: return {ReflectedShaderDataType::FeedbackTexture, StorageResourceAccess::None};

			default: throw std::runtime_error("Failed to find a valid resource type");
		}
	}

	ResourceDimension ExtractDimension(D3D_SRV_DIMENSION dimension)
	{
		switch (dimension)
		{
			case D3D_SRV_DIMENSION_TEXTURE1D: return ResourceDimension::Texture1D;
			case D3D_SRV_DIMENSION_TEXTURE1DARRAY: return ResourceDimension::Texture1DArray;
			case D3D_SRV_DIMENSION_TEXTURE2D: return ResourceDimension::Texture2D;
			case D3D_SRV_DIMENSION_TEXTURE2DARRAY: return ResourceDimension::Texture2DArray;
			case D3D_SRV_DIMENSION_TEXTURE2DMS: return ResourceDimension::Texture2DMS;
			case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY: return ResourceDimension::Texture2DMSArray;
			case D3D_SRV_DIMENSION_TEXTURE3D: return ResourceDimension::Texture3D;
			case D3D_SRV_DIMENSION_TEXTURECUBE: return ResourceDimension::TextureCube;
			case D3D_SRV_DIMENSION_TEXTURECUBEARRAY: return ResourceDimension::TextureCubeArray;
			default: return ResourceDimension::None;
		}
	}

	void ExtractResource(ShaderReflectionData &reflectionData, D3D12_SHADER_INPUT_BIND_DESC resource)
	{
		auto [dataType, storageAccess] = ExtractShaderInputType(resource.Type, resource.uFlags);

		ReflectedResource &reflectedResource	= reflectionData.Resources.emplace_back();
		reflectedResource.Type					= dataType;
		reflectedResource.Name					= resource.Name;
		reflectedResource.StorageResourceAccess = storageAccess;
		reflectedResource.Dimension				= ExtractDimension(resource.Dimension);
		reflectedResource.BindingPoint			= resource.BindPoint;
		reflectedResource.BindingCount			= resource.BindCount;
		reflectedResource.RegisterSpace			= resource.Space;
	}

	void ShaderModuleD3D12::ReflectShader(Microsoft::WRL::ComPtr<IDxcUtils> utils, Microsoft::WRL::ComPtr<IDxcResult> compileResult)
	{
		m_ReflectionData = {};

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
			ExtractAttribute(m_ReflectionData.Inputs, input);
		}

		for (UINT i = 0; i < shaderDesc.OutputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC output;
			shaderReflection->GetOutputParameterDesc(i, &output);
			ExtractAttribute(m_ReflectionData.Outputs, output);
		}

		for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			shaderReflection->GetResourceBindingDesc(i, &bindDesc);
			ExtractResource(m_ReflectionData, bindDesc);
		}
	}
}	 // namespace Nexus::Graphics

#endif