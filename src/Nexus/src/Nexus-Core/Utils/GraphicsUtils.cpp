#include "Nexus-Core/Utils/GraphicsUtils.hpp"
#include "Nexus-Core/Caching/CachedShader.hpp"

namespace
{
	Nexus::Ref<Nexus::Graphics::IShaderModule> TryLoadCachedShader(Nexus::Graphics::IGraphicsDevice *device,
																   const std::string				&source,
																   const std::string				&name,
																   Nexus::Graphics::ShaderStage		 stage,
																   const std::string				&outputDirectory,
																   Nexus::Graphics::ShaderLanguage	 language)
	{
		std::size_t hash		   = std::hash<std::string> {}(source);
		std::string languageString = ShaderLanguageToString(language);
		std::string filepath	   = outputDirectory + std::string("/cache/shaders/") + languageString + "/" + name;

		bool									   shaderCreated = false;
		Nexus::Ref<Nexus::Graphics::IShaderModule> module		 = nullptr;

		if (std::filesystem::exists(filepath))
		{
			Nexus::Graphics::CachedShader cache = Nexus::Graphics::CachedShader::LoadFromFile(filepath);
			if (cache.Validate(hash))
			{
				const Nexus::Graphics::ShaderModuleSpecification &shaderSpec = cache.GetShaderSpecification();
				module														 = device->CreateShaderModule(shaderSpec);
				shaderCreated												 = true;
			}
		}

		if (!shaderCreated)
		{
			module								= Nexus::Utils::CreateShaderModuleFromSpirvSource(device, source, name, outputDirectory, stage);
			Nexus::Graphics::CachedShader cache = Nexus::Graphics::CachedShader::FromModule(module->GetModuleSpecification(), hash);
			cache.Cache(filepath);
		}

		return module;
	}
}	 // namespace

namespace Nexus::Utils
{
	Ref<Graphics::ISurface> CreateSurfaceForWindow(Graphics::IGraphicsDevice *graphicsDevice, Nexus::IWindow *window)
	{
#if defined(WIN32)
		auto win32Info = window->GetWin32Info();
		return graphicsDevice->CreateSurfaceFromWin32(win32Info.hWND, win32Info.hDC, win32Info.hINSTANCE);
#else if defined(__linux__)
		auto x11Info = window->GetX11Info();
		return graphicsDevice->CreateSurfaceFromX11(x11Info.display, x11Info.screen, x11Info.window);
#endif

		throw std::runtime_error("Failed to create surface for window: Unsupported platform");
	}

	Ref<Graphics::IShaderModule> CreateShaderModuleFromSpirvFile(Graphics::IGraphicsDevice *graphicsDevice,
																 const std::string		   &filepath,
																 const std::string		   &outputDirectory,
																 Graphics::ShaderStage		stage)
	{
		std::string shaderSource = Nexus::FileSystem::ReadFileToString(filepath);
		return CreateShaderModuleFromSpirvSource(graphicsDevice, shaderSource, filepath, outputDirectory, stage);
	}

	Ref<Graphics::IShaderModule> CreateShaderModuleFromSpirvSource(Graphics::IGraphicsDevice *graphicsDevice,
																   const std::string		 &source,
																   const std::string		 &name,
																   const std::string		 &outputDirectory,
																   Graphics::ShaderStage	  stage)
	{
		Graphics::ShaderModuleSpecification moduleSpec;

		auto startTime = std::chrono::system_clock::now();

		Graphics::ShaderGenerator generator;
		std::string				  errorMessage;

		Graphics::ShaderGenerationOptions options;
		options.Stage		 = stage;
		options.ShaderName	 = name;
		options.OutputFormat = graphicsDevice->GetSupportedShaderFormat();

		auto result = generator.Generate(source, options);

		if (!result.Successful)
		{
			NX_ERROR(result.Error);
			throw std::runtime_error(result.Error);
		}

		auto endTime   = std::chrono::system_clock::now();
		auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

		moduleSpec.DebugName		= name;
		moduleSpec.Source			= result.Source;
		moduleSpec.ShadingStage		= stage;
		moduleSpec.SpirvBinary		= result.SpirvBinary;
		moduleSpec.InputAttributes	= result.InputAttributes;
		moduleSpec.OutputAttributes = result.OutputAttributes;

		return graphicsDevice->CreateShaderModule(moduleSpec);
	}

	Ref<Graphics::IShaderModule> GetOrCreateCachedShaderFromSpirvSource(Graphics::IGraphicsDevice *graphicsDevice,
																		const std::string		  &source,
																		const std::string		  &name,
																		const std::string		  &outputDirectory,
																		Graphics::ShaderStage	   stage)
	{
		Graphics::ShaderLanguage language = graphicsDevice->GetSupportedShaderFormat();
		return TryLoadCachedShader(graphicsDevice, source, name, stage, outputDirectory, language);
	}

	Ref<Graphics::IShaderModule> GetOrCreateCachedShaderFromSpirvFile(Graphics::IGraphicsDevice *graphicsDevice,
																	  const std::string			&filepath,
																	  const std::string			&outputDirectory,
																	  Graphics::ShaderStage		 stage)
	{
		std::string source = Nexus::FileSystem::ReadFileToString(filepath);
		return GetOrCreateCachedShaderFromSpirvSource(graphicsDevice, source, filepath, outputDirectory, stage);
	}
}	 // namespace Nexus::Utils