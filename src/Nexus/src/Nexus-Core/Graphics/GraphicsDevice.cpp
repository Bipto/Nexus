#include "Nexus-Core/nxpch.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/Graphics/ShaderGenerator.hpp"
#include "Nexus-Core/Graphics/ShaderUtils.hpp"
#include "Nexus-Core/Logging/Log.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "stb_image.h"

#include "Nexus-Core/Caching/CachedShader.hpp"

// graphics headers
#if defined(NX_PLATFORM_OPENGL)
	#include "Platform/OpenGL/GraphicsDeviceOpenGL.hpp"
#endif

#if defined(NX_PLATFORM_D3D12)
	#include "Platform/D3D12/GraphicsDeviceD3D12.hpp"
#endif

#if defined(NX_PLATFORM_VULKAN)
	#include "Platform/Vulkan/GraphicsDeviceVk.hpp"
#endif

namespace Nexus::Graphics
{
	Ref<ShaderModule> GraphicsDevice::CreateShaderModuleFromSpirvFile(const std::string &filepath, ShaderStage stage)
	{
		std::string shaderSource = Nexus::FileSystem::ReadFileToString(filepath);
		return CreateShaderModuleFromSpirvSource(shaderSource, filepath, stage);
	}

	Ref<ShaderModule> GraphicsDevice::CreateShaderModuleFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage)
	{
		ShaderModuleSpecification moduleSpec;
		ResourceSetSpecification  resourceSetSpec;

		auto startTime = std::chrono::system_clock::now();

		ShaderGenerator generator;
		std::string		errorMessage;

		ShaderGenerationOptions options;
		options.Stage		 = stage;
		options.ShaderName	 = name;
		options.OutputFormat = GetSupportedShaderFormat();

		auto result = generator.Generate(source, options, resourceSetSpec);

		if (!result.Successful)
		{
			throw std::runtime_error(result.Error);
		}

		auto endTime   = std::chrono::system_clock::now();
		auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

		moduleSpec.Name				= name;
		moduleSpec.Source			= result.Source;
		moduleSpec.ShadingStage		= stage;
		moduleSpec.SpirvBinary		= result.SpirvBinary;
		moduleSpec.InputAttributes	= result.InputAttributes;
		moduleSpec.OutputAttributes = result.OutputAttributes;

		return CreateShaderModule(moduleSpec, resourceSetSpec);
	}

	Ref<ShaderModule> GraphicsDevice::GetOrCreateCachedShaderFromSpirvSource(const std::string &source, const std::string &name, ShaderStage stage)
	{
		return TryLoadCachedShader(source, name, stage, GetSupportedShaderFormat());
	}

	Ref<ShaderModule> GraphicsDevice::GetOrCreateCachedShaderFromSpirvFile(const std::string &filepath, ShaderStage stage)
	{
		std::string source = Nexus::FileSystem::ReadFileToString(filepath);
		return GetOrCreateCachedShaderFromSpirvSource(source, filepath, stage);
	}

	void GraphicsDevice::ImmediateSubmit(std::function<void(Ref<CommandList>)> &&function)
	{
		if (!m_ImmediateCommandList)
		{
			m_ImmediateCommandList = CreateCommandList();
		}

		m_ImmediateCommandList->Begin();
		function(m_ImmediateCommandList);
		m_ImmediateCommandList->End();
		SubmitCommandLists(&m_ImmediateCommandList, 1, nullptr);
		WaitForIdle();
	}

	void GraphicsDevice::WriteToTexture(Ref<Texture> texture,
										uint32_t	 arrayLayer,
										uint32_t	 mipLevel,
										uint32_t	 x,
										uint32_t	 y,
										uint32_t	 z,
										uint32_t	 width,
										uint32_t	 height,
										const void	*data,
										size_t		 size)
	{
		DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access					  = BufferMemoryAccess::Upload;
		bufferDesc.Usage					  = BUFFER_USAGE_NONE;
		bufferDesc.SizeInBytes			   = size;
		bufferDesc.StrideInBytes			  = size;
		Ref<DeviceBuffer>			  buffer  = CreateDeviceBuffer(bufferDesc);
		Ref<CommandList>			  cmdList = CreateCommandList();

		buffer->SetData(data, 0, size);

		cmdList->Begin();

		BufferTextureCopyDescription copyDesc = {};
		copyDesc.BufferHandle				  = buffer;
		copyDesc.BufferOffset				  = 0;
		copyDesc.TextureHandle				  = texture;
		copyDesc.TextureSubresource			  = {.X			 = x,
												 .Y			 = y,
												 .Z			 = z,
												 .Width		 = width,
												 .Height	 = height,
												 .Depth		 = 1,
												 .MipLevel	 = mipLevel,
												 .ArrayLayer = arrayLayer,
												 .Aspect	 = Graphics::ImageAspect::Colour};

		cmdList->CopyBufferToTexture(copyDesc);

		cmdList->End();
		SubmitCommandLists(&cmdList, 1, nullptr);
		WaitForIdle();

	}	 // namespace Nexus::Graphics

	std::vector<char> GraphicsDevice::ReadFromTexture(Ref<Texture> texture,
													  uint32_t	   arrayLayer,
													  uint32_t	   mipLevel,
													  uint32_t	   x,
													  uint32_t	   y,
													  uint32_t	   z,
													  uint32_t	   width,
													  uint32_t	   height)
	{
		size_t bufferSize = width * height * GetPixelFormatSizeInBytes(texture->GetSpecification().Format);

		DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access				   = BufferMemoryAccess::Readback;
		bufferDesc.Usage				   = BUFFER_USAGE_NONE;
		bufferDesc.SizeInBytes			   = bufferSize;
		bufferDesc.StrideInBytes		   = bufferSize;

		Ref<DeviceBuffer>			  buffer  = CreateDeviceBuffer(bufferDesc);
		Ref<CommandList>			  cmdList = CreateCommandList();

		cmdList->Begin();

		BufferTextureCopyDescription copyDesc = {};
		copyDesc.BufferHandle				  = buffer;
		copyDesc.BufferOffset				  = 0;
		copyDesc.TextureHandle				  = texture;
		copyDesc.TextureSubresource =
			{.X = x, .Y = y, .Z = arrayLayer, .Width = width, .Height = height, .Depth = 1, .Aspect = Graphics::ImageAspect::Colour};

		cmdList->CopyTextureToBuffer(copyDesc);

		cmdList->End();
		SubmitCommandLists(&cmdList, 1, nullptr);
		WaitForIdle();

		return buffer->GetData(0, bufferSize);
	}

	bool GraphicsDevice::Validate()
	{
		return true;
	}

	void GraphicsDevice::SetName(const std::string &name)
	{
		m_Name = name;
	}

	const std::string &GraphicsDevice::GetName()
	{
		return m_Name;
	}

	Ref<ShaderModule> GraphicsDevice::TryLoadCachedShader(const std::string &source,
														  const std::string &name,
														  ShaderStage		 stage,
														  ShaderLanguage	 language)
	{
		std::size_t hash		   = Utils::Hash(source);
		std::string languageString = ShaderLanguageToString(language);
		std::string filepath	   = Nexus::GetApplication()->GetApplicationPath() + std::string("/cache/shaders/") + languageString + "/" + name;

		bool			  shaderCreated = false;
		Ref<ShaderModule> module		= nullptr;

		if (std::filesystem::exists(filepath))
		{
			CachedShader cache = CachedShader::LoadFromFile(filepath);
			if (cache.Validate(hash))
			{
				const ShaderModuleSpecification &shaderSpec	  = cache.GetShaderSpecification();
				const ResourceSetSpecification	&resourceSpec = cache.GetResourceSpecification();
				module										  = CreateShaderModule(shaderSpec, resourceSpec);
				shaderCreated								  = true;
			}
		}

		if (!shaderCreated)
		{
			module			   = CreateShaderModuleFromSpirvSource(source, name, stage);
			CachedShader cache = CachedShader::FromModule(module->GetModuleSpecification(), module->GetResourceSetSpecification(), hash);
			cache.Cache(filepath);
		}

		return module;
	}

	Ref<Texture> GraphicsDevice::CreateTexture2D(const char *filepath, bool generateMips, bool srgb)
	{
		int receivedChannels  = 0;
		int width			  = 0;
		int height			  = 0;
		int requestedChannels = 4;

		stbi_set_flip_vertically_on_load(true);

		TextureSpecification spec;
		unsigned char		*data = stbi_load(filepath, &width, &height, &receivedChannels, requestedChannels);
		spec.Width				  = (uint32_t)width;
		spec.Height				  = (uint32_t)height;
		spec.Format				  = PixelFormat::R8_G8_B8_A8_UNorm;
		spec.MipLevels			  = 1;

		if (srgb)
		{
			spec.Format = PixelFormat::R8_G8_B8_A8_UNorm_SRGB;
		}

		if (generateMips)
		{
			uint32_t mipCount = Nexus::Graphics::MipmapGenerator::GetMaximumNumberOfMips(spec.Width, spec.Height);
			spec.MipLevels	  = mipCount;
		}

		size_t bufferSize = spec.Width * spec.Height * GetPixelFormatSizeInBytes(spec.Format);
		auto   texture	  = Ref<Texture>(CreateTexture(spec));
		WriteToTexture(texture, 0, 0, 0, 0, 0, spec.Width, spec.Height, data, bufferSize);
		stbi_image_free(data);

		if (generateMips)
		{
			Nexus::Graphics::MipmapGenerator mipGenerator(this);

			for (uint32_t i = 1; i < spec.MipLevels; i++)
			{
				auto [width, height]			  = Utils::GetMipSize(spec.Width, spec.Height, i);
				std::vector<char> pixels		  = mipGenerator.GenerateMip(texture, i, i - 1);
				WriteToTexture(texture, 0, i, 0, 0, 0, width, height, pixels.data(), pixels.size());
			}
		}

		return texture;
	}

	Ref<Texture> GraphicsDevice::CreateTexture2D(const std::string &filepath, bool generateMips, bool srgb)
	{
		return CreateTexture2D(filepath.c_str(), generateMips, srgb);
	}

	Ref<ResourceSet> GraphicsDevice::CreateResourceSet(Ref<Pipeline> pipeline)
	{
		return CreateResourceSet(pipeline->GetResourceSetSpecification());
	}
}	 // namespace Nexus::Graphics