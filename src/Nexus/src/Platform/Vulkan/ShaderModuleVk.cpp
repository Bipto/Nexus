#if defined(NX_PLATFORM_VULKAN)

	#include "ShaderModuleVk.hpp"

	#include "GraphicsDeviceVk.hpp"

	#include "spirv_cross.hpp"

namespace Nexus::Graphics
{
	ShaderModuleVk::ShaderModuleVk(const ShaderModuleSpecification &shaderModuleSpec,
								   const ResourceSetSpecification  &resourceSpec,
								   GraphicsDeviceVk				   *device)
		: ShaderModule(shaderModuleSpec, resourceSpec),
		  m_GraphicsDevice(device)
	{
		CreateShaderModule();
	}

	ShaderModuleVk::~ShaderModuleVk()
	{
		vkDestroyShaderModule(m_GraphicsDevice->GetVkDevice(), m_ShaderModule, nullptr);
	}

	VkShaderModule ShaderModuleVk::GetShaderModule()
	{
		return m_ShaderModule;
	}

	void ExtractResource(ReflectedResource			 &reflectedResource,
						 const spirv_cross::Resource &spirvResource,
						 spirv_cross::Compiler		 &compiler,
						 ReflectedShaderDataType	  dataType)
	{
		uint32_t					 set	 = compiler.get_decoration(spirvResource.id, spv::DecorationDescriptorSet);
		uint32_t					 binding = compiler.get_decoration(spirvResource.id, spv::DecorationBinding);
		const spirv_cross::SPIRType &type	 = compiler.get_type(spirvResource.type_id);

		reflectedResource.Type			= dataType;
		reflectedResource.Name			= spirvResource.name;
		reflectedResource.DescriptorSet = set;
		reflectedResource.BindingPoint	= binding;

		if (!type.array.empty())
		{
			reflectedResource.BindingCount = type.array[0];
		}
		else
		{
			reflectedResource.BindingCount = 1;
		}

		if (dataType == ReflectedShaderDataType::Texture || dataType == ReflectedShaderDataType::CombinedImageSampler)
		{
			switch (type.image.dim)
			{
				case spv::Dim::Dim1D:
				{
					if (type.image.arrayed)
					{
						reflectedResource.Dimension = ResourceDimension::Texture1DArray;
						break;
					}
					else
					{
						reflectedResource.Dimension = ResourceDimension::Texture1D;
						break;
					}
				}
				case spv::Dim::Dim2D:
				{
					if (type.image.arrayed)
					{
						if (type.image.ms)
						{
							reflectedResource.Dimension = ResourceDimension::Texture2DMSArray;
						}
						else
						{
							reflectedResource.Dimension = ResourceDimension::Texture2DArray;
						}
						break;
					}
					else
					{
						if (type.image.ms)
						{
							reflectedResource.Dimension = ResourceDimension::Texture2DMS;
						}
						else
						{
							reflectedResource.Dimension = ResourceDimension::Texture2D;
						}
						break;
					}
				}
				case spv::Dim::Dim3D:
				{
					reflectedResource.Dimension = ResourceDimension::Texture3D;
					break;
				}
				case spv::Dim::DimBuffer:
				{
					dataType = ReflectedShaderDataType::TextureBuffer;
					break;
				}
				case spv::Dim::DimCube:
				{
					if (type.image.arrayed)
					{
						reflectedResource.Dimension = ResourceDimension::TextureCubeArray;
						break;
					}
					else
					{
						reflectedResource.Dimension = ResourceDimension::TextureCube;
						break;
					}
				}
				case spv::Dim::DimRect:
				{
					reflectedResource.Dimension = ResourceDimension::TextureRectangle;
					break;
				}
				case spv::Dim::DimSubpassData:
				{
					reflectedResource.Dimension = ResourceDimension::None;
					break;
				}
				default: throw std::runtime_error("Failed to find a valid resource dimension");
			}
		}
		else if (dataType == ReflectedShaderDataType::Sampler)
		{
			if (type.image.depth)
			{
				dataType = ReflectedShaderDataType::ComparisonSampler;
			}
		}
	}

	ShaderReflectionData ShaderModuleVk::Reflect() const
	{
		ShaderReflectionData reflectionData = {};

		spirv_cross::Compiler		 compiler(m_ModuleSpecification.SpirvBinary);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const auto &uniformBuffer : resources.uniform_buffers)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, uniformBuffer, compiler, ReflectedShaderDataType::UniformBuffer);
		}

		for (const auto &storageBuffer : resources.storage_buffers)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, storageBuffer, compiler, ReflectedShaderDataType::StorageBuffer);
		}

		for (const auto &subpassInput : resources.subpass_inputs)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, subpassInput, compiler, ReflectedShaderDataType::InputAttachment);
		}

		for (const auto &storageImage : resources.storage_images)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, storageImage, compiler, ReflectedShaderDataType::StorageImage);
		}

		for (const auto &sampledImage : resources.sampled_images)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, sampledImage, compiler, ReflectedShaderDataType::CombinedImageSampler);
		}

		for (const auto &atomicCounter : resources.atomic_counters)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, atomicCounter, compiler, ReflectedShaderDataType::AtomicUint);
		}

		for (const auto &accelerationStructure : resources.acceleration_structures)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, accelerationStructure, compiler, ReflectedShaderDataType::AccelerationStructure);
		}

		for (const auto &pushConstant : resources.push_constant_buffers)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, pushConstant, compiler, ReflectedShaderDataType::PushConstant);
		}

		for (const auto &separateImage : resources.separate_images)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, separateImage, compiler, ReflectedShaderDataType::Texture);
		}

		for (const auto &separateSampler : resources.acceleration_structures)
		{
			ReflectedResource &reflectedResource = reflectionData.Resources.emplace_back();
			ExtractResource(reflectedResource, separateSampler, compiler, ReflectedShaderDataType::Sampler);
		}

		return reflectionData;
	}

	void ShaderModuleVk::CreateShaderModule()
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType					= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext					= nullptr;
		createInfo.codeSize					= m_ModuleSpecification.SpirvBinary.size() * sizeof(uint32_t);
		createInfo.pCode					= m_ModuleSpecification.SpirvBinary.data();

		if (vkCreateShaderModule(m_GraphicsDevice->GetVkDevice(), &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
	}
}	 // namespace Nexus::Graphics

#endif