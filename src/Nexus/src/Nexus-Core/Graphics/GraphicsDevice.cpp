#include "Nexus-Core/nxpch.hpp"

#include "GraphicsDevice.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/Logging/Log.hpp"
#include "ShaderGenerator.hpp"
#include "ShaderUtils.hpp"
#include "stb_image.h"

#include "CachedShader.hpp"
#include "CachedTexture.hpp"

namespace Nexus::Graphics
{
	GraphicsDevice::GraphicsDevice(const GraphicsDeviceSpecification &specification, IWindow *window, const SwapchainSpecification &swapchainSpec)
	{
		m_Window		= window;
		m_Specification = specification;
	}

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

	IWindow *GraphicsDevice::GetPrimaryWindow()
	{
		return m_Window;
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
		SubmitCommandList(m_ImmediateCommandList);
	}

	const GraphicsDeviceSpecification &GraphicsDevice::GetSpecification() const
	{
		return m_Specification;
	}

	Ref<ShaderModule> GraphicsDevice::TryLoadCachedShader(const std::string &source,
														  const std::string &name,
														  ShaderStage		 stage,
														  ShaderLanguage	 language)
	{
		std::size_t hash		   = Utils::Hash(source);
		std::string languageString = ShaderLanguageToString(language);
		std::string filepath	   = "cache/shaders/" + languageString + "/" + name;

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

	Ref<Texture2D> GraphicsDevice::CreateTexture2D(const char *filepath, bool generateMips)
	{
		int desiredChannels	 = 4;
		int receivedChannels = 0;
		int width			 = 0;
		int height			 = 0;

		Texture2DSpecification spec;
		spec.Format			= PixelFormat::R8_G8_B8_A8_UNorm;
		unsigned char *data = stbi_load(filepath, &width, &height, &receivedChannels, desiredChannels);
		spec.Width			= (uint32_t)width;
		spec.Height			= (uint32_t)height;

		if (generateMips)
		{
			uint32_t mipCount = Nexus::Graphics::MipmapGenerator::GetMaximumNumberOfMips(spec.Width, spec.Height);
			spec.MipLevels	  = mipCount;
		}
		else
		{
			spec.MipLevels = 1;
		}

		auto texture = CreateTexture2D(spec);
		texture->SetData(data, 0, 0, 0, spec.Width, spec.Height);

		if (generateMips)
		{
			Nexus::Graphics::MipmapGenerator mipGenerator(this);

			for (uint32_t i = 1; i < spec.MipLevels; i++)
			{
				auto [width, height]			  = Utils::GetMipSize(spec.Width, spec.Height, i);
				std::vector<unsigned char> pixels = mipGenerator.GenerateMip(texture, i, i - 1);
				texture->SetData(pixels.data(), i, 0, 0, width, height);
			}
		}

		stbi_image_free(data);
		return texture;
	}

	Ref<Texture2D> GraphicsDevice::CreateTexture2D(const std::string &filepath, bool generateMips)
	{
		return CreateTexture2D(filepath.c_str(), generateMips);
	}

	Ref<ResourceSet> GraphicsDevice::CreateResourceSet(Ref<Pipeline> pipeline)
	{
		return CreateResourceSet(pipeline->GetPipelineDescription().ResourceSetSpec);
	}
}	 // namespace Nexus::Graphics