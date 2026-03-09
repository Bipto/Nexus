#include "RHI/GraphicsDevice.hpp"
#include "RHI/IGraphicsAPI.hpp"

#if defined(NX_TEST_GRAPHICS)

void CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI					   api,
								std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	  &graphicsAPI,
								std::unique_ptr<Nexus::Graphics::IGraphicsDevice> &device,
								Nexus::Ref<Nexus::Graphics::ICommandQueue>		  &queue)
{
	Nexus::Graphics::GraphicsAPICreateInfo apiCreateInfo = {};
	apiCreateInfo.API									 = api;
	apiCreateInfo.Debug									 = true;

	graphicsAPI = std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(Nexus::Graphics::IGraphicsAPI::CreateAPI(apiCreateInfo));

	std::vector<std::shared_ptr<Nexus::Graphics::IPhysicalDevice>> physicalDevices = graphicsAPI->GetPhysicalDevices();
	device = std::unique_ptr<Nexus::Graphics::IGraphicsDevice>(graphicsAPI->CreateGraphicsDevice(physicalDevices[0]));

	const auto &queueFamilies = device->GetQueueFamilies();

	for (const auto &queueFamily : queueFamilies)
	{
		if (queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Graphics))
		{
			Nexus::Graphics::CommandQueueDescription queueDesc = {};
			queueDesc.QueueFamilyIndex						   = queueFamily.QueueFamily;
			queueDesc.QueueIndex							   = 0;
			queue											   = device->CreateCommandQueue(queueDesc);
			return;
		}
	}
}

	#if defined(NX_PLATFORM_OPENGL)
TEST(CreateGraphicsDeviceOpenGL, Successful)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	  api	 = nullptr;
	std::unique_ptr<Nexus::Graphics::IGraphicsDevice> device = nullptr;
	Nexus::Ref<Nexus::Graphics::ICommandQueue>		  queue	 = nullptr;
	CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI::OpenGL, api, device, queue);
	EXPECT_TRUE(device->Validate());
}
	#endif

	#if defined(NX_PLATFORM_D3D12)
TEST(CreateGraphicsDeviceD3D12, Successful)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	  api	 = nullptr;
	std::unique_ptr<Nexus::Graphics::IGraphicsDevice> device = nullptr;

	Nexus::Ref<Nexus::Graphics::ICommandQueue> queue = nullptr;
	CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI::D3D12, api, device, queue);
	EXPECT_TRUE(device->Validate());
}
	#endif

	#if defined(NX_PLATFORM_VULKAN)
TEST(CreateGraphicsDeviceVulkan, Successful)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	  api	 = nullptr;
	std::unique_ptr<Nexus::Graphics::IGraphicsDevice> device = nullptr;

	Nexus::Ref<Nexus::Graphics::ICommandQueue> queue = nullptr;
	CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI::Vulkan, api, device, queue);
	EXPECT_TRUE(device->Validate());
}
	#endif

bool RunTextureCopyTest(Nexus::Graphics::GraphicsAPI api)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	  graphicsAPI = nullptr;
	std::unique_ptr<Nexus::Graphics::IGraphicsDevice> device	  = nullptr;
	Nexus::Ref<Nexus::Graphics::ICommandQueue>		  queue		  = nullptr;
	CreateGraphicsAPIAndDevice(api, graphicsAPI, device, queue);

	Nexus::Ref<Nexus::Graphics::ICommandList> cmdList = queue->CreateCommandList();

	Nexus::Graphics::DeviceBufferDescription bufferDesc		= {};
	bufferDesc.Usage										= Nexus::Graphics::BufferUsage_None;
	bufferDesc.Access										= Nexus::Graphics::BufferMemoryAccess::Upload;
	bufferDesc.SizeInBytes									= sizeof(uint32_t);
	bufferDesc.StrideInBytes								= sizeof(uint32_t);
	Nexus::Ref<Nexus::Graphics::IDeviceBuffer> uploadBuffer = device->CreateDeviceBuffer(bufferDesc);

	bufferDesc.Access										  = Nexus::Graphics::BufferMemoryAccess::Readback;
	Nexus::Ref<Nexus::Graphics::IDeviceBuffer> readbackBuffer = device->CreateDeviceBuffer(bufferDesc);

	Nexus::Graphics::TextureDescription textureSpec = {};
	textureSpec.Width								= 1;
	textureSpec.Height								= 1;
	textureSpec.DepthOrArrayLayers					= 1;
	textureSpec.MipLevels							= 1;

	Nexus::Ref<Nexus::Graphics::ITexture> sourceTexture = device->CreateTexture(textureSpec);
	Nexus::Ref<Nexus::Graphics::ITexture> destTexture	= device->CreateTexture(textureSpec);

	uint32_t col = 0xFF0000FF;
	uploadBuffer->SetData(&col, 0, sizeof(col));

	cmdList->Begin();

	Nexus::Graphics::BufferTextureCopyDescription uploadCopyDesc = {};
	uploadCopyDesc.BufferHandle									 = uploadBuffer;
	uploadCopyDesc.TextureHandle								 = sourceTexture;
	uploadCopyDesc.BufferOffset									 = 0;
	uploadCopyDesc.TextureOffset								 = {0, 0, 0};
	uploadCopyDesc.TextureExtent								 = {1, 1};
	uploadCopyDesc.MipLevel										 = 0;
	cmdList->CopyBufferToTexture(uploadCopyDesc);

	Nexus::Graphics::TextureCopyDescription textureCopyDesc = {};
	textureCopyDesc.Source									= sourceTexture;
	textureCopyDesc.Destination								= destTexture;
	textureCopyDesc.SourceOffset							= {0, 0, 0};
	textureCopyDesc.SourceMipLevel							= 0;
	textureCopyDesc.DestinationOffset						= {0, 0, 0};
	textureCopyDesc.DestinationMipLevel						= 0;
	textureCopyDesc.Extent									= {1, 1};
	cmdList->CopyTextureToTexture(textureCopyDesc);

	Nexus::Graphics::BufferTextureCopyDescription readbackCopyDesc = {};
	readbackCopyDesc.BufferHandle								   = readbackBuffer;
	readbackCopyDesc.TextureHandle								   = destTexture;
	readbackCopyDesc.BufferOffset								   = 0;
	readbackCopyDesc.MipLevel									   = 0;
	readbackCopyDesc.TextureOffset								   = {0, 0, 0};
	readbackCopyDesc.TextureExtent								   = {1, 1};
	readbackCopyDesc.MipLevel									   = 0;
	cmdList->CopyTextureToBuffer(readbackCopyDesc);

	cmdList->End();

	queue->SubmitCommandList(cmdList);
	device->WaitForIdle();

	std::vector<char> pixel = readbackBuffer->GetData(0, sizeof(uint32_t));
	return memcmp(&col, pixel.data(), sizeof(uint32_t)) == 0;
}

	#if defined(NX_PLATFORM_OPENGL)
TEST(CopyTextureToBufferOpenGL, Successful)
{
	EXPECT_TRUE(RunTextureCopyTest(Nexus::Graphics::GraphicsAPI::OpenGL));
}
	#endif

	#if defined(NX_PLATFORM_D3D12)
TEST(CopyTextureToBufferD3D12, Successful)
{
	EXPECT_TRUE(RunTextureCopyTest(Nexus::Graphics::GraphicsAPI::D3D12));
}
	#endif

	#if defined(NX_PLATFORM_VULKAN)
TEST(CopyTextureToBufferVulkan, Successful)
{
	EXPECT_TRUE(RunTextureCopyTest(Nexus::Graphics::GraphicsAPI::Vulkan));
}
	#endif

#endif