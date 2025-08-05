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
		Reflect();
	}

	ShaderModuleVk::~ShaderModuleVk()
	{
		vkDestroyShaderModule(m_GraphicsDevice->GetVkDevice(), m_ShaderModule, nullptr);
	}

	VkShaderModule ShaderModuleVk::GetShaderModule()
	{
		return m_ShaderModule;
	}

	void ExtractAttribute(Attribute &attribute, const spirv_cross::Resource &spirvResource, spirv_cross::Compiler &compiler)
	{
		attribute.Binding = compiler.get_decoration(spirvResource.id, spv::DecorationLocation);
		attribute.Name	  = spirvResource.name;

		const spirv_cross::SPIRType &type = compiler.get_type(spirvResource.type_id);

		switch (type.basetype)
		{
			case spirv_cross::SPIRType::BaseType::Boolean:
			{
				switch (type.vecsize)
				{
					case 1:
					{
						attribute.Type = ReflectedShaderDataType::Bool;
						break;
					}
					case 2:
					{
						attribute.Type = ReflectedShaderDataType::Bool2;
						break;
					}
					case 3:
					{
						attribute.Type = ReflectedShaderDataType::Bool3;
						break;
					}
					case 4:
					{
						attribute.Type = ReflectedShaderDataType::Bool4;
						break;
					}
					default: throw std::runtime_error("Failed to find a valid vector size");
				}
				break;
			}
			case spirv_cross::SPIRType::BaseType::Int:
			{
				switch (type.vecsize)
				{
					case 1:
					{
						attribute.Type = ReflectedShaderDataType::Int;
						break;
					}
					case 2:
					{
						attribute.Type = ReflectedShaderDataType::Int2;
						break;
					}
					case 3:
					{
						attribute.Type = ReflectedShaderDataType::Int3;
						break;
					}
					case 4:
					{
						attribute.Type = ReflectedShaderDataType::Int4;
						break;
					}
					default: throw std::runtime_error("Failed to find a valid vector size");
				}
				break;
			}
			case spirv_cross::SPIRType::BaseType::UInt:
			{
				switch (type.vecsize)
				{
					case 1:
					{
						attribute.Type = ReflectedShaderDataType::UInt;
						break;
					}
					case 2:
					{
						attribute.Type = ReflectedShaderDataType::UInt2;
						break;
					}
					case 3:
					{
						attribute.Type = ReflectedShaderDataType::UInt3;
						break;
					}
					case 4:
					{
						attribute.Type = ReflectedShaderDataType::UInt4;
						break;
					}
					default: throw std::runtime_error("Failed to find a valid vector size");
				}
				break;
			}
			case spirv_cross::SPIRType::BaseType::Float:
			{
				// we are reflecting a matrix
				if (type.columns > 1)
				{
					switch (type.vecsize)
					{
						case 2:
						{
							switch (type.columns)
							{
								case 2:
								{
									attribute.Type = ReflectedShaderDataType::Mat2x2;
									break;
								}
								case 3:
								{
									attribute.Type = ReflectedShaderDataType::Mat2x3;
									break;
								}
								case 4:
								{
									attribute.Type = ReflectedShaderDataType::Mat2x4;
									break;
								}
								default: throw std::runtime_error("Failed to find a valid matrix type");
							}
						}
						case 3:
						{
							switch (type.columns)
							{
								case 2:
								{
									attribute.Type = ReflectedShaderDataType::Mat3x2;
									break;
								}
								case 3:
								{
									attribute.Type = ReflectedShaderDataType::Mat3x3;
									break;
								}
								case 4:
								{
									attribute.Type = ReflectedShaderDataType::Mat3x4;
									break;
								}
								default: throw std::runtime_error("Failed to find a valid matrix type");
							}
						}
						case 4:
						{
							switch (type.columns)
							{
								case 2:
								{
									attribute.Type = ReflectedShaderDataType::Mat4x2;
									break;
								}
								case 3:
								{
									attribute.Type = ReflectedShaderDataType::Mat4x3;
									break;
								}
								case 4:
								{
									attribute.Type = ReflectedShaderDataType::Mat4x4;
									break;
								}
								default: throw std::runtime_error("Failed to find a valid matrix type");
							}
						}
						default: throw std::runtime_error("Failed to find a valid matrix type");
					}
				}
				// we are reflecting a vector
				else
				{
					switch (type.vecsize)
					{
						case 1:
						{
							attribute.Type = ReflectedShaderDataType::Float;
							break;
						}
						case 2:
						{
							attribute.Type = ReflectedShaderDataType::Float2;
							break;
						}
						case 3:
						{
							attribute.Type = ReflectedShaderDataType::Float3;
							break;
						}
						case 4:
						{
							attribute.Type = ReflectedShaderDataType::Float4;
							break;
						}
						default: throw std::runtime_error("Failed to find a valid vector size");
					}
				}
				break;
			}
			case spirv_cross::SPIRType::BaseType::Double:
			{
				// we are reflecting a matrix
				if (type.columns > 1)
				{
					switch (type.vecsize)
					{
						case 2:
						{
							switch (type.columns)
							{
								case 2:
								{
									attribute.Type = ReflectedShaderDataType::DMat2x2;
									break;
								}
								case 3:
								{
									attribute.Type = ReflectedShaderDataType::DMat2x3;
									break;
								}
								case 4:
								{
									attribute.Type = ReflectedShaderDataType::DMat2x4;
									break;
								}
								default: throw std::runtime_error("Failed to find a valid matrix type");
							}
						}
						case 3:
						{
							switch (type.columns)
							{
								case 2:
								{
									attribute.Type = ReflectedShaderDataType::DMat3x2;
									break;
								}
								case 3:
								{
									attribute.Type = ReflectedShaderDataType::DMat3x3;
									break;
								}
								case 4:
								{
									attribute.Type = ReflectedShaderDataType::DMat3x4;
									break;
								}
								default: throw std::runtime_error("Failed to find a valid matrix type");
							}
						}
						case 4:
						{
							switch (type.columns)
							{
								case 2:
								{
									attribute.Type = ReflectedShaderDataType::DMat4x2;
									break;
								}
								case 3:
								{
									attribute.Type = ReflectedShaderDataType::DMat4x3;
									break;
								}
								case 4:
								{
									attribute.Type = ReflectedShaderDataType::DMat4x4;
									break;
								}
								default: throw std::runtime_error("Failed to find a valid matrix type");
							}
						}
						default: throw std::runtime_error("Failed to find a valid matrix type");
					}
				}
				// we are reflecting a vector
				else
				{
					switch (type.vecsize)
					{
						case 1:
						{
							attribute.Type = ReflectedShaderDataType::Double;
							break;
						}
						case 2:
						{
							attribute.Type = ReflectedShaderDataType::Double2;
							break;
						}
						case 3:
						{
							attribute.Type = ReflectedShaderDataType::Double3;
							break;
						}
						case 4:
						{
							attribute.Type = ReflectedShaderDataType::Double4;
							break;
						}
						default: throw std::runtime_error("Failed to find a valid vector size");
					}
				}
				break;
			}
		}
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
					reflectedResource.StorageResourceAccess = StorageResourceAccess::Read;
					dataType								= ReflectedShaderDataType::UniformTextureBuffer;
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
		else if (dataType == ReflectedShaderDataType::StorageBuffer)
		{
			auto flags		 = compiler.get_buffer_block_flags(spirvResource.id);
			bool isReadonly	 = flags.get(spv::DecorationNonWritable);
			bool isWriteOnly = flags.get(spv::DecorationNonReadable);

			if (isReadonly)
			{
				reflectedResource.StorageResourceAccess = StorageResourceAccess::Read;
			}
			else if (isWriteOnly)
			{
				reflectedResource.StorageResourceAccess = StorageResourceAccess::Write;
			}
			else
			{
				reflectedResource.StorageResourceAccess = StorageResourceAccess::ReadWrite;
			}
		}
		else if (dataType == ReflectedShaderDataType::StorageImage)
		{
			auto flags		 = compiler.get_decoration_bitset(spirvResource.id);
			bool isReadonly	 = flags.get(spv::DecorationNonWritable);
			bool isWriteOnly = flags.get(spv::DecorationNonReadable);

			if (isReadonly)
			{
				reflectedResource.StorageResourceAccess = StorageResourceAccess::Read;
			}
			else if (isWriteOnly)
			{
				reflectedResource.StorageResourceAccess = StorageResourceAccess::Write;
			}
			else
			{
				reflectedResource.StorageResourceAccess = StorageResourceAccess::ReadWrite;
			}
		}
	}

	ShaderReflectionData ShaderModuleVk::Reflect() const
	{
		ShaderReflectionData reflectionData = {};

		spirv_cross::Compiler		 compiler(m_ModuleSpecification.SpirvBinary);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const auto &input : resources.stage_inputs)
		{
			Attribute &attribute = reflectionData.Inputs.emplace_back();
			ExtractAttribute(attribute, input, compiler);
		}

		for (const auto &output : resources.stage_outputs)
		{
			Attribute &attribute = reflectionData.Outputs.emplace_back();
			ExtractAttribute(attribute, output, compiler);
		}

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