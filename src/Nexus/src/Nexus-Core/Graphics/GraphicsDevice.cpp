#include "GraphicsDevice.hpp"

#include "Nexus-Core/FileSystem/FileSystem.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/Logging/Log.hpp"
#include "Nexus-Core/nxpch.hpp"
#include "ShaderGenerator.hpp"
#include "ShaderUtils.hpp"
#include "stb_image.h"

#include "yaml-cpp/yaml.h"

namespace Nexus::Graphics
{
	GraphicsDevice::GraphicsDevice(const GraphicsDeviceSpecification &specification, Window *window, const SwapchainSpecification &swapchainSpec)
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
		moduleSpec.Stage			= stage;
		moduleSpec.SpirvBinary		= result.SpirvBinary;
		moduleSpec.InputAttributes	= result.InputAttributes;
		moduleSpec.OutputAttributes = result.OutputAttributes;

		return CreateShaderModule(moduleSpec, resourceSetSpec);
	}

	Ref<ShaderModule> GraphicsDevice::GetOrCreateCachedShader(const std::string &source, const std::string &name, ShaderStage stage)
	{
		std::size_t		  hash	 = std::hash<std::string> {}(source);
		Ref<ShaderModule> module = CreateShaderModuleFromSpirvSource(source, name, stage);

		// std::string outputText = std::to_string(hash) + std::string("\n") + module->GetModuleSpecification().Source;

		ShaderLanguage language		  = GetSupportedShaderFormat();
		std::string	   languageString = ShaderLanguageToString(language);

		std::string filepath = "cache/shaders/" + languageString + "/" + name;

		// FileSystem::WriteFileAbsolute(filepath, outputText);

		const ShaderModuleSpecification &spec = module->GetModuleSpecification();

		YAML::Emitter root;
		root << YAML::BeginMap;
		root << YAML::Key << "Name" << YAML::Value << spec.Name;
		root << YAML::Key << "Hash" << YAML::Value << hash;
		root << YAML::Key << "Source" << YAML::Value << spec.Source;
		root << YAML::Key << "Stage" << YAML::Value << (uint32_t)spec.Stage;
		root << YAML::Key << "SPIRV" << YAML::Value << spec.SpirvBinary;
		root << YAML::EndMap;

		FileSystem::WriteFileAbsolute(filepath, root.c_str());

		return module;
	}

	Window *GraphicsDevice::GetPrimaryWindow()
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
			uint32_t						 mipsToGenerate = spec.MipLevels - 1;
			Nexus::Graphics::MipmapGenerator mipGenerator(this);
			mipGenerator.GenerateMips(texture, mipsToGenerate);
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