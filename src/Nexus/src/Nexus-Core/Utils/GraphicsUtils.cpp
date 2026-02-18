#include "Nexus-Core/Utils/GraphicsUtils.hpp"
#include "Nexus-Core/Caching/CachedShader.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"

#include "Nexus-Core/Graphics/MipmapGenerator.hpp"

#include "stb_image.h"

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

	Ref<Graphics::ITexture> CreateTexture2D(Ref<Graphics::ICommandQueue> commandQueue, const char *filepath, bool generateMips, bool srgb)
	{
		Graphics::IGraphicsDevice *device = commandQueue->GetGraphicsDevice();

		int receivedChannels  = 0;
		int width			  = 0;
		int height			  = 0;
		int requestedChannels = 4;

		stbi_set_flip_vertically_on_load(true);

		Graphics::TextureDescription spec;
		unsigned char				*data = stbi_load(filepath, &width, &height, &receivedChannels, requestedChannels);
		spec.Width						  = (uint32_t)width;
		spec.Height						  = (uint32_t)height;
		spec.Format						  = Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		spec.MipLevels					  = 1;

		if (srgb)
		{
			spec.Format = Graphics::PixelFormat::R8_G8_B8_A8_UNorm_SRGB;
		}

		if (generateMips)
		{
			uint32_t mipCount = Graphics::MipmapGenerator::GetMaximumNumberOfMips(spec.Width, spec.Height);
			spec.MipLevels	  = mipCount;
		}

		auto   texture	  = Ref<Graphics::ITexture>(device->CreateTexture(spec));
		size_t bufferSize = spec.Width * spec.Height * GetPixelFormatSizeInBytes(spec.Format);

		commandQueue->WriteToTexture(texture, 0, 0, 0, 0, spec.Width, spec.Height, data, bufferSize);

		stbi_image_free(data);

		if (generateMips)
		{
			Graphics::MipmapGenerator mipGenerator(device, commandQueue);

			for (uint32_t i = 1; i < spec.MipLevels; i++)
			{
				auto [width, height]	 = Utils::GetMipSize(spec.Width, spec.Height, i);
				std::vector<char> pixels = mipGenerator.GenerateMip(texture, i, i - 1, 0);
				commandQueue->WriteToTexture(texture, i, 0, 0, 0, width, height, pixels.data(), pixels.size());
			}
		}

		return texture;
	}

	Ref<Graphics::ITexture> CreateTexture2D(Ref<Graphics::ICommandQueue> commandQueue, const std::string &filepath, bool generateMips, bool srgb)
	{
		return CreateTexture2D(commandQueue, filepath.c_str(), generateMips, srgb);
	}

	std::pair<Ref<Graphics::ITexture>, Ref<Graphics::ITextureView>> CreateTexture2DWithView(Ref<Graphics::ICommandQueue> commandQueue,
																							const char					*filepath,
																							bool						 generateMips,
																							bool						 srgb)
	{
		Graphics::IGraphicsDevice *device = commandQueue->GetGraphicsDevice();

		Ref<Graphics::ITexture> texture = CreateTexture2D(commandQueue, filepath, generateMips, srgb);

		Graphics::TextureViewDescription viewDesc = {};
		viewDesc.TargetTexture					  = texture;
		viewDesc.Format							  = texture->GetPixelFormat();
		viewDesc.Range							  = {.BaseMipLevel	 = 0,
													 .LevelCount	 = texture->GetMipLevels(),
													 .BaseArrayLayer = 0,
													 .LayerCount	 = texture->GetDepthOrArrayLayers()};
		std::string viewName					  = filepath + std::string(" - View");
		viewDesc.DebugName						  = viewName;
		Ref<Graphics::ITextureView> textureView	  = device->CreateTextureView(viewDesc);

		return {texture, textureView};
	}

	std::pair<Ref<Graphics::ITexture>, Ref<Graphics::ITextureView>> CreateTexture2DWithView(Ref<Graphics::ICommandQueue> commandQueue,
																							const std::string			&filepath,
																							bool						 generateMips,
																							bool						 srgb)
	{
		return CreateTexture2DWithView(commandQueue, filepath.c_str(), generateMips, srgb);
	}
}	 // namespace Nexus::Utils