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

#include <stb_image_write.h>

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

		auto startTime = std::chrono::system_clock::now();

		ShaderGenerator generator;
		std::string		errorMessage;

		ShaderGenerationOptions options;
		options.Stage		 = stage;
		options.ShaderName	 = name;
		options.OutputFormat = GetSupportedShaderFormat();

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

		return CreateShaderModule(moduleSpec);
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

	bool GraphicsDevice::Validate()
	{
		return true;
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
				const ShaderModuleSpecification &shaderSpec = cache.GetShaderSpecification();
				module										= CreateShaderModule(shaderSpec);
				shaderCreated								= true;
			}
		}

		if (!shaderCreated)
		{
			module			   = CreateShaderModuleFromSpirvSource(source, name, stage);
			CachedShader cache = CachedShader::FromModule(module->GetModuleSpecification(), hash);
			cache.Cache(filepath);
		}

		return module;
	}

	Ref<Texture> GraphicsDevice::CreateTexture2D(Ref<ICommandQueue> commandQueue, const char *filepath, bool generateMips, bool srgb)
	{
		int receivedChannels  = 0;
		int width			  = 0;
		int height			  = 0;
		int requestedChannels = 4;

		stbi_set_flip_vertically_on_load(true);

		TextureDescription spec;
		unsigned char	  *data = stbi_load(filepath, &width, &height, &receivedChannels, requestedChannels);
		spec.Width				= (uint32_t)width;
		spec.Height				= (uint32_t)height;
		spec.Format				= PixelFormat::R8_G8_B8_A8_UNorm;
		spec.MipLevels			= 1;

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
		commandQueue->WriteToTexture(texture, 0, 0, 0, 0, spec.Width, spec.Height, data, bufferSize);

		stbi_write_png("0.png", spec.Width, spec.Height, 4, data, spec.Width * 4);

		stbi_image_free(data);

		if (generateMips)
		{
			Nexus::Graphics::MipmapGenerator mipGenerator(this, commandQueue);

			for (uint32_t i = 1; i < spec.MipLevels; i++)
			{
				auto [width, height]	 = Utils::GetMipSize(spec.Width, spec.Height, i);
				std::vector<char> pixels = mipGenerator.GenerateMip(texture, i, i - 1, 0);
				commandQueue->WriteToTexture(texture, i, 0, 0, 0, width, height, pixels.data(), pixels.size());

				std::stringstream ss;
				ss << std::to_string(i) << ".png";
				stbi_write_png(ss.str().c_str(), width, height, 4, pixels.data(), width * 4);
			}
		}

		return texture;
	}

	Ref<Texture> GraphicsDevice::CreateTexture2D(Ref<ICommandQueue> commandQueue, const std::string &filepath, bool generateMips, bool srgb)
	{
		return CreateTexture2D(commandQueue, filepath.c_str(), generateMips, srgb);
	}

	std::pair<Ref<Texture>, Ref<ITextureView>> GraphicsDevice::CreateTexture2DWithView(Ref<ICommandQueue> commandQueue,
																					   const char		 *filepath,
																					   bool				  generateMips,
																					   bool				  srgb)
	{
		Ref<Texture> texture = CreateTexture2D(commandQueue, filepath, generateMips, srgb);

		TextureViewDescription viewDesc = {};
		viewDesc.TargetTexture			= texture;
		viewDesc.Format					= texture->GetPixelFormat();
		viewDesc.Range					= {.BaseMipLevel   = 0,
										   .LevelCount	   = texture->GetMipLevels(),
										   .BaseArrayLayer = 0,
										   .LayerCount	   = texture->GetDepthOrArrayLayers()};
		std::string viewName			= filepath + std::string(" - View");
		viewDesc.DebugName				= viewName;
		Ref<ITextureView> textureView	= CreateTextureView(viewDesc);

		return {texture, textureView};
	}

	std::pair<Ref<Texture>, Ref<ITextureView>> GraphicsDevice::CreateTexture2DWithView(Ref<ICommandQueue> commandQueue,
																					   const std::string &filepath,
																					   bool				  generateMips,
																					   bool				  srgb)
	{
		return CreateTexture2DWithView(commandQueue, filepath.c_str(), generateMips, srgb);
	}

	Ref<Framebuffer> GraphicsDevice::CreateFramebuffer(const FramebufferTextureCreateDescription &desc)
	{
		FramebufferTextureSetDescription framebufferDesc = {};

		for (size_t i = 0; i < desc.ColourAttachmentFormats.size(); i++)
		{
			PixelFormat format = desc.ColourAttachmentFormats.at(i);

			TextureDescription textureDesc = {};
			textureDesc.Width			   = desc.Width;
			textureDesc.Height			   = desc.Height;
			textureDesc.Type			   = TextureType::Texture2D;
			textureDesc.Usage			   = Graphics::TextureUsage_ColourAttachment;
			textureDesc.Samples			   = desc.Samples;
			textureDesc.Format			   = format;

			Ref<Texture> texture = CreateTexture(textureDesc);

			FramebufferTextureDescription &framebufferTextureDesc = framebufferDesc.ColourAttachments.emplace_back();
			framebufferTextureDesc.TargetTexture				  = texture;
			framebufferTextureDesc.BaseArrayLayer				  = 0;
			framebufferTextureDesc.LayerCount					  = 1;
			framebufferTextureDesc.MipLevel						  = 0;
		}

		if (desc.DepthAttachmentFormat.has_value())
		{
			TextureDescription textureDesc = {};
			textureDesc.Width			   = desc.Width;
			textureDesc.Height			   = desc.Height;
			textureDesc.Type			   = TextureType::Texture2D;
			textureDesc.Usage			   = Graphics::TextureUsage_DepthStencilAttachment;
			textureDesc.Samples			   = desc.Samples;
			textureDesc.Format			   = desc.DepthAttachmentFormat.value();

			Ref<Texture> texture = CreateTexture(textureDesc);

			FramebufferTextureDescription framebufferTextureDesc = {};
			framebufferTextureDesc.TargetTexture				 = texture;
			framebufferTextureDesc.BaseArrayLayer				 = 0;
			framebufferTextureDesc.LayerCount					 = 1;
			framebufferTextureDesc.MipLevel						 = 0;

			framebufferDesc.DepthAttachment = framebufferTextureDesc;
		}

		return CreateFramebuffer(framebufferDesc);
	}
}	 // namespace Nexus::Graphics
