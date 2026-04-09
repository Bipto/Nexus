#include "Nexus-Core/Utils/GraphicsUtils.hpp"
#include "Nexus-Core/Caching/CachedShader.hpp"
#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/Graphics/ShaderGenerator.hpp"
#include "Nexus-Core/Utils/Utils.hpp"
#include "RHI/GraphicsDevice.hpp"

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
				const Nexus::Graphics::ShaderModuleDescription &shaderSpec = cache.GetShaderModuleDescription();
				module													   = device->CreateShaderModule(shaderSpec);
				shaderCreated											   = true;
			}
		}

		if (!shaderCreated)
		{
			module								= Nexus::Utils::CreateShaderModuleFromSpirvSource(device, source, name, outputDirectory, stage);
			Nexus::Graphics::CachedShader cache = Nexus::Graphics::CachedShader::FromModule(module->GetModuleDescription(), hash);
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
		Graphics::ShaderModuleDescription moduleSpec;

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

		Utils::WriteToTexture(commandQueue, texture, 0, 0, 0, 0, spec.Width, spec.Height, data, bufferSize);

		stbi_image_free(data);

		if (generateMips)
		{
			Graphics::MipmapGenerator mipGenerator(device, commandQueue);

			for (uint32_t i = 1; i < spec.MipLevels; i++)
			{
				auto [width, height]	 = Utils::GetMipSize(spec.Width, spec.Height, i);
				std::vector<char> pixels = mipGenerator.GenerateMip(texture, i, i - 1, 0);
				WriteToTexture(commandQueue, texture, i, 0, 0, 0, width, height, pixels.data(), pixels.size());
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

	Ref<Graphics::IFramebuffer> CreateFramebuffer(Graphics::IGraphicsDevice *device, const Graphics::FramebufferTextureCreateDescription &desc)
	{
		Graphics::FramebufferTextureSetDescription framebufferDesc = {};

		for (size_t i = 0; i < desc.ColourAttachmentFormats.size(); i++)
		{
			Graphics::PixelFormat format = desc.ColourAttachmentFormats.at(i);

			Graphics::TextureDescription textureDesc = {};
			textureDesc.Width						 = desc.Width;
			textureDesc.Height						 = desc.Height;
			textureDesc.Type						 = Graphics::TextureType::Texture2D;
			textureDesc.Usage						 = Graphics::TextureUsage_ColourAttachment;
			textureDesc.Samples						 = desc.Samples;
			textureDesc.Format						 = format;

			Ref<Graphics::ITexture> colourAttachment = device->CreateTexture(textureDesc);

			Graphics::FramebufferColourAttachmentDescription &framebufferTextureDesc = framebufferDesc.ColourAttachments.emplace_back();
			framebufferTextureDesc.ColourAttachment.TargetTexture					 = colourAttachment;
			framebufferTextureDesc.ColourAttachment.BaseArrayLayer					 = 0;
			framebufferTextureDesc.ColourAttachment.LayerCount						 = 1;
			framebufferTextureDesc.ColourAttachment.MipLevel						 = 0;

			// create a resolve attachment if using multi-sampling
			if (textureDesc.Samples > 1)
			{
				Graphics::TextureDescription resolveTextureDesc = {};
				resolveTextureDesc.Width						= desc.Width;
				resolveTextureDesc.Height						= desc.Height;
				resolveTextureDesc.Type							= Graphics::TextureType::Texture2D;
				resolveTextureDesc.Usage						= Graphics::TextureUsage_ColourAttachment;
				resolveTextureDesc.Samples						= 1;
				resolveTextureDesc.Format						= format;

				Ref<Graphics::ITexture> resolveAttachment = device->CreateTexture(resolveTextureDesc);

				Graphics::FramebufferTextureDescription resolveAttachmentDesc = {};
				resolveAttachmentDesc.TargetTexture							  = resolveAttachment;
				resolveAttachmentDesc.BaseArrayLayer						  = 0;
				resolveAttachmentDesc.LayerCount							  = 1;
				resolveAttachmentDesc.MipLevel								  = 0;
				framebufferTextureDesc.ResolveAttachment					  = resolveAttachmentDesc;
			}
		}

		if (desc.DepthAttachmentFormat.has_value())
		{
			Graphics::TextureDescription textureDesc = {};
			textureDesc.Width						 = desc.Width;
			textureDesc.Height						 = desc.Height;
			textureDesc.Type						 = Graphics::TextureType::Texture2D;
			textureDesc.Usage						 = Graphics::TextureUsage_DepthStencilAttachment;
			textureDesc.Samples						 = desc.Samples;
			textureDesc.Format						 = desc.DepthAttachmentFormat.value();

			Ref<Graphics::ITexture> texture = device->CreateTexture(textureDesc);

			Graphics::FramebufferTextureDescription framebufferTextureDesc = {};
			framebufferTextureDesc.TargetTexture						   = texture;
			framebufferTextureDesc.BaseArrayLayer						   = 0;
			framebufferTextureDesc.LayerCount							   = 1;
			framebufferTextureDesc.MipLevel								   = 0;

			framebufferDesc.DepthAttachment = framebufferTextureDesc;
		}

		return device->CreateFramebuffer(framebufferDesc);
	}

	void WriteToTexture(Ref<Graphics::ICommandQueue> commandQueue,
						Ref<Graphics::ITexture>		 texture,
						uint32_t					 mipLevel,
						uint32_t					 x,
						uint32_t					 y,
						uint32_t					 z,
						uint32_t					 width,
						uint32_t					 height,
						const void					*data,
						size_t						 size)
	{
		Graphics::IGraphicsDevice *device = commandQueue->GetGraphicsDevice();

		Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access							 = Graphics::BufferMemoryAccess::Upload;
		bufferDesc.Usage							 = Graphics::BufferUsage_None;
		bufferDesc.SizeInBytes						 = size;
		bufferDesc.StrideInBytes					 = size;
		Ref<Graphics::IDeviceBuffer> buffer			 = device->CreateDeviceBuffer(bufferDesc);
		Ref<Graphics::ICommandList>	 cmdList		 = commandQueue->CreateCommandList();

		buffer->SetData(data, 0, size);

		cmdList->Begin();

		Graphics::BufferTextureCopyDescription copyDesc = {};
		copyDesc.BufferHandle							= buffer.get();
		copyDesc.BufferOffset							= 0;
		copyDesc.BufferRowLength						= 0;
		copyDesc.BufferImageHeight						= 0;
		copyDesc.TextureHandle							= texture;
		copyDesc.TextureOffset							= {.X = (int32_t)x, .Y = (int32_t)y, .Z = (int32_t)z};
		copyDesc.TextureExtent							= {.Width = width, .Height = height};
		copyDesc.MipLevel								= mipLevel;
		cmdList->CopyBufferToTexture(copyDesc);

		cmdList->End();
		commandQueue->SubmitCommandList(cmdList);
		device->WaitForIdle();
	}

	std::vector<char> ReadFromTexture(Ref<Graphics::ICommandQueue> commandQueue,
									  Ref<Graphics::ITexture>	   texture,
									  uint32_t					   mipLevel,
									  uint32_t					   x,
									  uint32_t					   y,
									  uint32_t					   z,
									  uint32_t					   width,
									  uint32_t					   height)
	{
		Graphics::IGraphicsDevice *device = commandQueue->GetGraphicsDevice();

		Graphics::SubresourceFootprint footprint = texture->GetSubresourceFootprint(0, mipLevel);

		Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access							 = Graphics::BufferMemoryAccess::Readback;
		bufferDesc.Usage							 = Graphics::BufferUsage_None;
		bufferDesc.SizeInBytes						 = footprint.Size;
		bufferDesc.StrideInBytes					 = footprint.Size;

		Ref<Graphics::IDeviceBuffer> buffer	 = device->CreateDeviceBuffer(bufferDesc);
		Ref<Graphics::ICommandList>	 cmdList = commandQueue->CreateCommandList();

		cmdList->Begin();

		Graphics::BufferTextureCopyDescription copyDesc = {};
		copyDesc.BufferHandle							= buffer.get();
		copyDesc.BufferOffset							= 0;
		copyDesc.BufferRowLength						= 0;
		copyDesc.BufferImageHeight						= 0;
		copyDesc.TextureHandle							= texture;
		copyDesc.TextureOffset							= {.X = (int32_t)x, .Y = (int32_t)y, .Z = (int32_t)z};
		copyDesc.TextureExtent							= {.Width = width, .Height = height};
		copyDesc.MipLevel								= mipLevel;
		cmdList->CopyTextureToBuffer(copyDesc);

		cmdList->End();
		commandQueue->SubmitCommandList(cmdList);
		device->WaitForIdle();

		return buffer->GetData(0, footprint.Size);
	}

	void WriteToBuffer(Ref<Graphics::ICommandQueue> commandQueue, Ref<Graphics::IDeviceBuffer> buffer, const void *data, size_t offset, size_t size)
	{
		Graphics::IGraphicsDevice *device = commandQueue->GetGraphicsDevice();

		Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access							 = Nexus::Graphics::BufferMemoryAccess::Upload;
		bufferDesc.SizeInBytes						 = size;
		bufferDesc.StrideInBytes					 = size;
		Ref<Graphics::IDeviceBuffer> uploadBuffer	 = device->CreateDeviceBuffer(bufferDesc);

		uploadBuffer->SetData(data, 0, size);

		Graphics::BufferCopy bufferCopy = {};
		bufferCopy.Size					= size;
		bufferCopy.ReadOffset			= 0;
		bufferCopy.WriteOffset			= offset;

		Graphics::BufferCopyDescription bufferCopyDesc = {};
		bufferCopyDesc.Source						   = uploadBuffer.get();
		bufferCopyDesc.Destination					   = buffer.get();
		bufferCopyDesc.Copies						   = {bufferCopy};

		Ref<Graphics::ICommandList> cmdList = commandQueue->CreateCommandList();
		cmdList->Begin();
		cmdList->CopyBufferToBuffer(bufferCopyDesc);
		cmdList->End();

		commandQueue->SubmitCommandList(cmdList);
		device->WaitForIdle();
	}

	std::vector<char> ReadFromBuffer(Ref<Graphics::ICommandQueue> commandQueue, Ref<Graphics::IDeviceBuffer> buffer, size_t offset)
	{
		size_t dataSize = buffer->GetSizeInBytes() - offset;

		Graphics::IGraphicsDevice *device = commandQueue->GetGraphicsDevice();

		Graphics::DeviceBufferDescription bufferDesc = {};
		bufferDesc.Access							 = Nexus::Graphics::BufferMemoryAccess::Readback;
		bufferDesc.SizeInBytes						 = dataSize;
		bufferDesc.StrideInBytes					 = dataSize;
		Ref<Graphics::IDeviceBuffer> readbackBuffer	 = device->CreateDeviceBuffer(bufferDesc);

		Graphics::BufferCopy bufferCopy = {};
		bufferCopy.Size					= dataSize;
		bufferCopy.ReadOffset			= offset;
		bufferCopy.WriteOffset			= 0;

		Graphics::BufferCopyDescription bufferCopyDesc = {};
		bufferCopyDesc.Source						   = buffer.get();
		bufferCopyDesc.Destination					   = readbackBuffer.get();
		bufferCopyDesc.Copies						   = {bufferCopy};

		Ref<Graphics::ICommandList> cmdList = commandQueue->CreateCommandList();
		cmdList->Begin();
		cmdList->CopyBufferToBuffer(bufferCopyDesc);
		cmdList->End();

		commandQueue->SubmitCommandList(cmdList);

		device->WaitForIdle();

		return readbackBuffer->GetData(0, dataSize);
	}
}	 // namespace Nexus::Utils