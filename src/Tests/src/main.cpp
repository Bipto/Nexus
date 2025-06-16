#include <gtest/gtest.h>

#include "Nexus-Core/Graphics/Circle.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

#include "Nexus-Core/Events/EventHandler.hpp"

#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/IGraphicsAPI.hpp"

TEST(Point2D, To)
{
	Nexus::Point2D<int>	  value(5, 7);
	Nexus::Point2D<float> result = value.To<float>();

	EXPECT_EQ(result.X, 5.0f);
	EXPECT_EQ(result.Y, 7.0f);
}

TEST(Point2D, Deconstruct)
{
	Nexus::Point2D<int> value(5, 7);

	int x, y;
	value.Deconstruct(&x, &y);

	EXPECT_EQ(value.X, x);
	EXPECT_EQ(value.Y, y);
}

TEST(Point3D, To)
{
	Nexus::Point3D<int>	  value(5, 7, 2);
	Nexus::Point3D<float> result = value.To<float>();

	EXPECT_EQ(result.X, 5.0f);
	EXPECT_EQ(result.Y, 7.0f);
	EXPECT_EQ(result.Z, 2.0f);
}

TEST(Point3D, Deconstruct)
{
	Nexus::Point3D<int> value(5, 7, 2);

	int x, y, z;
	value.Deconstruct(&x, &y, &z);

	EXPECT_EQ(value.X, x);
	EXPECT_EQ(value.Y, y);
	EXPECT_EQ(value.Z, z);
}

TEST(Circle, To)
{
	const Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25);
	Nexus::Graphics::Circle<float>	   result = c.To<float>();

	EXPECT_EQ(result.GetPosition().X, 50.0f);
	EXPECT_EQ(result.GetPosition().Y, 50.0f);
}

TEST(Circle, ContainsValid)
{
	Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25.0f);
	bool						 result = c.Contains(Nexus::Point2D<int>(50, 50));

	EXPECT_EQ(result, true);
}

TEST(Circle, ContainsInvalid)
{
	Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25.0f);
	bool						 result = c.Contains(Nexus::Point2D<int>(20, 50));

	EXPECT_EQ(result, false);
}

TEST(Circle, Deconstruct)
{
	Nexus::Graphics::Circle<int> c(Nexus::Point2D<int>(50, 50), 25.0f);

	int x, y, radius;
	c.Deconstruct(&x, &y, &radius);

	EXPECT_EQ(x, 50);
	EXPECT_EQ(y, 50);
	EXPECT_EQ(radius, 25);
}

TEST(ScopedEvent, Behaviour)
{
	Nexus::EventHandler<> eventHandler;

	EXPECT_EQ(eventHandler.GetDelegateCount(), 0);

	{
		Nexus::ScopedEvent<> event(&eventHandler, []() {});
		EXPECT_EQ(eventHandler.GetDelegateCount(), 1);
	}

	EXPECT_EQ(eventHandler.GetDelegateCount(), 0);
}

void CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI					  api,
								std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	 &graphicsAPI,
								std::unique_ptr<Nexus::Graphics::GraphicsDevice> &device)
{
	Nexus::Graphics::GraphicsAPICreateInfo apiCreateInfo = {};
	apiCreateInfo.API									 = api;
	apiCreateInfo.Debug									 = true;

	graphicsAPI = std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(Nexus::Graphics::IGraphicsAPI::CreateAPI(apiCreateInfo));

	std::vector<std::shared_ptr<Nexus::Graphics::IPhysicalDevice>> physicalDevices = graphicsAPI->GetPhysicalDevices();
	device = std::unique_ptr<Nexus::Graphics::GraphicsDevice>(graphicsAPI->CreateGraphicsDevice(physicalDevices[0]));
}

#if defined(NX_PLATFORM_OPENGL)
TEST(CreateGraphicsDeviceOpenGL, Valid)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	 api	= nullptr;
	std::unique_ptr<Nexus::Graphics::GraphicsDevice> device = nullptr;
	CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI::OpenGL, api, device);
	EXPECT_TRUE(device->Validate());
}
#endif

#if defined(NX_PLATFORM_D3D12)
TEST(CreateGraphicsDeviceD3D12, Valid)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	 api	= nullptr;
	std::unique_ptr<Nexus::Graphics::GraphicsDevice> device = nullptr;
	CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI::D3D12, api, device);
	EXPECT_TRUE(device->Validate());
}
#endif

#if defined(NX_PLATFORM_VULKAN)
TEST(CreateGraphicsDeviceVulkan, Valid)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	 api	= nullptr;
	std::unique_ptr<Nexus::Graphics::GraphicsDevice> device = nullptr;
	CreateGraphicsAPIAndDevice(Nexus::Graphics::GraphicsAPI::Vulkan, api, device);
	EXPECT_TRUE(device->Validate());
}
#endif

bool RunTextureCopyTest(Nexus::Graphics::GraphicsAPI api)
{
	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	 graphicsAPI = nullptr;
	std::unique_ptr<Nexus::Graphics::GraphicsDevice> device		 = nullptr;
	CreateGraphicsAPIAndDevice(api, graphicsAPI, device);

	Nexus::Ref<Nexus::Graphics::CommandList> cmdList = device->CreateCommandList();

	Nexus::Graphics::DeviceBufferDescription bufferDesc	   = {};
	bufferDesc.Usage									   = BUFFER_USAGE_NONE;
	bufferDesc.Access									   = Nexus::Graphics::BufferMemoryAccess::Upload;
	bufferDesc.SizeInBytes								   = sizeof(uint32_t);
	bufferDesc.StrideInBytes							   = sizeof(uint32_t);
	Nexus::Ref<Nexus::Graphics::DeviceBuffer> uploadBuffer = device->CreateDeviceBuffer(bufferDesc);

	bufferDesc.Access										 = Nexus::Graphics::BufferMemoryAccess::Readback;
	Nexus::Ref<Nexus::Graphics::DeviceBuffer> readbackBuffer = device->CreateDeviceBuffer(bufferDesc);

	Nexus::Graphics::TextureSpecification textureSpec = {};
	textureSpec.Width								  = 1;
	textureSpec.Height								  = 1;
	textureSpec.ArrayLayers							  = 1;
	textureSpec.MipLevels							  = 1;

	Nexus::Ref<Nexus::Graphics::Texture> sourceTexture = device->CreateTexture(textureSpec);
	Nexus::Ref<Nexus::Graphics::Texture> destTexture   = device->CreateTexture(textureSpec);

	uint32_t col = 0xFF0000FF;
	uploadBuffer->SetData(&col, 0, sizeof(col));

	cmdList->Begin();

	Nexus::Graphics::BufferTextureCopyDescription uploadCopyDesc = {};
	uploadCopyDesc.BufferHandle									 = uploadBuffer;
	uploadCopyDesc.TextureHandle								 = sourceTexture;
	uploadCopyDesc.BufferOffset									 = 0;
	uploadCopyDesc.TextureSubresource.X							 = 0;
	uploadCopyDesc.TextureSubresource.Y							 = 0;
	uploadCopyDesc.TextureSubresource.Z							 = 0;
	uploadCopyDesc.TextureSubresource.Width						 = 1;
	uploadCopyDesc.TextureSubresource.Height					 = 1;
	uploadCopyDesc.TextureSubresource.Depth						 = 1;
	uploadCopyDesc.TextureSubresource.Aspect					 = Nexus::Graphics::ImageAspect::Colour;
	cmdList->CopyBufferToTexture(uploadCopyDesc);

	Nexus::Graphics::TextureCopyDescription textureCopyDesc = {};
	textureCopyDesc.Source									= sourceTexture;
	textureCopyDesc.Destination								= destTexture;
	textureCopyDesc.SourceSubresource.X						= 0;
	textureCopyDesc.SourceSubresource.Y						= 0;
	textureCopyDesc.SourceSubresource.Z						= 0;
	textureCopyDesc.SourceSubresource.Width					= 1;
	textureCopyDesc.SourceSubresource.Height				= 1;
	textureCopyDesc.SourceSubresource.Depth					= 1;
	textureCopyDesc.SourceSubresource.MipLevel				= 0;
	textureCopyDesc.SourceSubresource.Aspect				= Nexus::Graphics::ImageAspect::Colour;
	textureCopyDesc.DestinationSubresource.X				= 0;
	textureCopyDesc.DestinationSubresource.Y				= 0;
	textureCopyDesc.DestinationSubresource.Z				= 0;
	textureCopyDesc.DestinationSubresource.Width			= 1;
	textureCopyDesc.DestinationSubresource.Height			= 1;
	textureCopyDesc.DestinationSubresource.Depth			= 1;
	textureCopyDesc.DestinationSubresource.MipLevel			= 0;
	textureCopyDesc.DestinationSubresource.Aspect			= Nexus::Graphics::ImageAspect::Colour;
	cmdList->CopyTextureToTexture(textureCopyDesc);

	Nexus::Graphics::BufferTextureCopyDescription readbackCopyDesc = {};
	readbackCopyDesc.BufferHandle								   = readbackBuffer;
	readbackCopyDesc.TextureHandle								   = destTexture;
	readbackCopyDesc.BufferOffset								   = 0;
	readbackCopyDesc.TextureSubresource.X						   = 0;
	readbackCopyDesc.TextureSubresource.Y						   = 0;
	readbackCopyDesc.TextureSubresource.Z						   = 0;
	readbackCopyDesc.TextureSubresource.Width					   = 1;
	readbackCopyDesc.TextureSubresource.Height					   = 1;
	readbackCopyDesc.TextureSubresource.Depth					   = 1;
	readbackCopyDesc.TextureSubresource.Aspect					   = Nexus::Graphics::ImageAspect::Colour;
	cmdList->CopyTextureToBuffer(readbackCopyDesc);

	cmdList->End();

	device->SubmitCommandList(cmdList);
	device->WaitForIdle();

	std::vector<char> pixel = readbackBuffer->GetData(0, sizeof(uint32_t));
	return memcmp(&col, pixel.data(), sizeof(uint32_t)) == 0;
}

#if defined(NX_PLATFORM_OPENGL)
TEST(CopyTextureToBufferOpenGL, Valid)
{
	EXPECT_TRUE(RunTextureCopyTest(Nexus::Graphics::GraphicsAPI::OpenGL));
}
#endif

#if defined(NX_PLATFORM_D3D12)
TEST(CopyTextureToBufferD3D12, Valid)
{
	EXPECT_TRUE(RunTextureCopyTest(Nexus::Graphics::GraphicsAPI::D3D12));
}
#endif

#if defined(NX_PLATFORM_VULKAN)
TEST(CopyTextureToBufferVulkan, Valid)
{
	EXPECT_TRUE(RunTextureCopyTest(Nexus::Graphics::GraphicsAPI::Vulkan));
}
#endif