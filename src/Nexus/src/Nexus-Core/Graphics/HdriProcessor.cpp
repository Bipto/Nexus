#include "Nexus-Core/Graphics/HdriProcessor.hpp"

#include "Nexus-Core/Graphics/Framebuffer.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/MeshFactory.hpp"
#include "Nexus-Core/Graphics/Texture.hpp"
#include "stb_image.h"

const std::string HdriVertexShaderSource = "#version 450 core\n"

										   "layout(location = 0) in vec3 Position;\n"

										   "layout(location = 0) out vec3 LocalPos;\n"

										   "layout(binding = 0, set = 0) uniform Camera\n"
										   "{\n"
										   "    mat4 u_View;\n"
										   "    mat4 u_Projection;\n"
										   "};\n"

										   "void main()\n"
										   "{\n"
										   "    LocalPos = Position;\n"
										   "    gl_Position = u_Projection * u_View * vec4(Position, 1.0);\n"
										   "}";

const std::string HdriFragmentShaderSource = "#version 450 core\n"

											 "layout(location = 0) in vec3 LocalPos;\n"

											 "layout(location = 0) out vec4 FragColor;\n"

											 "layout(binding = 0, set = 1) uniform sampler2D equirectangularMap;\n"

											 "const vec2 invAtan = vec2(0.1591, 0.3183);\n"
											 "vec2 SampleSphericalMap(vec3 v)\n"
											 "{\n"
											 "    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n"
											 "    uv *= invAtan;\n"
											 "    uv += 0.5;\n"
											 "    return uv;\n"
											 "}\n"

											 "void main()\n"
											 "{\n"
											 "    vec2 uv = SampleSphericalMap(normalize(LocalPos));\n"
											 "    vec3 colour = texture(equirectangularMap, uv).rgb;\n"
											 "    FragColor = vec4(colour, 1.0);\n"
											 "}";

namespace Nexus::Graphics
{
	struct alignas(16) VB_UNIFORM_HDRI_PROCESSOR_CAMERA
	{
		glm::mat4 View		 = {};
		glm::mat4 Projection = {};
	};

	HdriProcessor::HdriProcessor(const std::string &filepath, GraphicsDevice *device) : m_Device(device)
	{
		stbi_set_flip_vertically_on_load(true);
		int	   channels			 = 0;
		int	   requestedChannels = 4;
		float *data				 = stbi_loadf(filepath.c_str(), &m_Width, &m_Height, &channels, requestedChannels);

		std::vector<unsigned char> pixels;
		size_t					   bufferSize = m_Width * m_Height * requestedChannels * sizeof(float);
		pixels.resize(bufferSize);
		memcpy(pixels.data(), data, pixels.size());
		stbi_image_free(data);

		if (m_Device->GetGraphicsAPI() == GraphicsAPI::OpenGL)
		{
			Utils::FlipPixelsHorizontally(pixels, m_Width, m_Height, Graphics::PixelFormat::R32_G32_B32_A32_Float);
		}

		Nexus::Graphics::Texture2DSpecification textureSpec {};
		textureSpec.Width  = m_Width;
		textureSpec.Height = m_Height;
		textureSpec.Usage  = {Nexus::Graphics::TextureUsage::Sampled};
		textureSpec.Format = Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;
		m_HdriImage		   = m_Device->CreateTexture2D(textureSpec);

		m_HdriImage->SetData(pixels.data(), 0, 0, 0, m_Width, m_Height);
	}

	Ref<Cubemap> HdriProcessor::Generate(uint32_t size)
	{
		Nexus::Graphics::FramebufferSpecification framebufferSpec = {};
		framebufferSpec.Width									  = size;
		framebufferSpec.Height									  = size;
		framebufferSpec.Samples									  = Nexus::Graphics::SampleCount::SampleCount1;
		framebufferSpec.ColorAttachmentSpecification			  = {PixelFormat::R32_G32_B32_A32_Float};
		framebufferSpec.DepthAttachmentSpecification			  = PixelFormat::D24_UNorm_S8_UInt;

		Ref<Framebuffer> framebuffer = m_Device->CreateFramebuffer(framebufferSpec);
		Ref<CommandList> commandList = m_Device->CreateCommandList();

		Nexus::Graphics::CubemapSpecification cubemapSpec {};
		cubemapSpec.Width	  = size;
		cubemapSpec.Height	  = size;
		cubemapSpec.MipLevels = 1;
		cubemapSpec.Format	  = Nexus::Graphics::PixelFormat::R32_G32_B32_A32_Float;
		Ref<Cubemap> cubemap  = m_Device->CreateCubemap(cubemapSpec);

		Nexus::Graphics::PipelineDescription pipelineDescription;
		pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::Back;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;
		pipelineDescription.VertexModule =
			m_Device->CreateShaderModuleFromSpirvSource(HdriVertexShaderSource, "hdri.vert.glsl", Nexus::Graphics::ShaderStage::Vertex);
		pipelineDescription.FragmentModule =
			m_Device->CreateShaderModuleFromSpirvSource(HdriFragmentShaderSource, "hdri.frag.glsl", Nexus::Graphics::ShaderStage::Fragment);
		pipelineDescription.ResourceSetSpec.UniformBuffers = {{"Camera", 0, 0}};
		pipelineDescription.ResourceSetSpec.SampledImages  = {{"equirectangularMap", 1, 0}};

		pipelineDescription.ColourFormats[0]  = framebufferSpec.ColorAttachmentSpecification.Attachments[0].TextureFormat;
		pipelineDescription.ColourTargetCount = 1;
		pipelineDescription.DepthFormat		  = framebufferSpec.DepthAttachmentSpecification.DepthFormat;

		pipelineDescription.Layouts	 = {Nexus::Graphics::VertexPositionTexCoordNormalTangentBitangent::GetLayout()};
		Ref<Pipeline>	 pipeline	 = m_Device->CreatePipeline(pipelineDescription);
		Ref<ResourceSet> resourceSet = m_Device->CreateResourceSet(pipeline);

		Nexus::Graphics::SamplerSpecification samplerSpec {};
		samplerSpec.AddressModeU = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeV = Nexus::Graphics::SamplerAddressMode::Clamp;
		samplerSpec.AddressModeW = Nexus::Graphics::SamplerAddressMode::Clamp;
		Ref<Sampler> sampler	 = m_Device->CreateSampler(samplerSpec);

		Nexus::Graphics::MeshFactory	  factory(m_Device);
		Nexus::Ref<Nexus::Graphics::Mesh> cube = factory.CreateCube();

		VB_UNIFORM_HDRI_PROCESSOR_CAMERA cameraUniforms;

		Nexus::Graphics::DeviceBufferDescription cameraUniformBufferDesc = {};
		cameraUniformBufferDesc.Type									 = Nexus::Graphics::DeviceBufferType::Uniform;
		cameraUniformBufferDesc.StrideInBytes							 = sizeof(VB_UNIFORM_HDRI_PROCESSOR_CAMERA);
		cameraUniformBufferDesc.SizeInBytes								 = sizeof(VB_UNIFORM_HDRI_PROCESSOR_CAMERA);
		cameraUniformBufferDesc.HostVisible								 = true;
		Ref<DeviceBuffer> uniformBuffer									 = Ref<DeviceBuffer>(m_Device->CreateDeviceBuffer(cameraUniformBufferDesc));

		for (uint32_t i = 0; i < 6; i++)
		{
			CubemapFace face	 = (CubemapFace)i;
			glm::vec3	position = glm::vec3(0.0f, 0.0f, 0.0f);

			float yaw = 0.0f, pitch = 0.0f;
			GetDirection(face, yaw, pitch);

			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::quat rotP = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat rotY = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 view = glm::mat4_cast(rotY) * glm::mat4_cast(rotP);

			if (!m_Device->IsUVOriginTopLeft())
			{
				view *= glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			}

			float	  fov		  = 90.0f;
			float	  aspectRatio = 1.0f;
			float	  camNear	  = 0.1f;
			float	  camFar	  = 100.0f;
			glm::mat4 projection  = glm::perspective(glm::radians(fov), aspectRatio, camNear, camFar);

			cameraUniforms.View		  = view;
			cameraUniforms.Projection = projection;

			uniformBuffer->SetData(&cameraUniforms, 0, sizeof(cameraUniforms));
			resourceSet->WriteUniformBuffer(uniformBuffer, "Camera");
			resourceSet->WriteCombinedImageSampler(m_HdriImage, sampler, "equirectangularMap");

			commandList->Begin();
			commandList->SetPipeline(pipeline);
			commandList->SetRenderTarget(Nexus::Graphics::RenderTarget(framebuffer));

			Nexus::Graphics::Viewport vp {};
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= size;
			vp.Height	= size;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			commandList->SetViewport(vp);

			Nexus::Graphics::Scissor scissor {};
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = size;
			scissor.Height = size;
			commandList->SetScissor(scissor);

			commandList->SetResourceSet(resourceSet);

			Graphics::VertexBufferView vertexBufferView = {};
			vertexBufferView.BufferHandle				= cube->GetVertexBuffer().get();
			vertexBufferView.Offset						= 0;
			vertexBufferView.Stride						= cube->GetVertexBuffer()->GetDescription().StrideInBytes;
			vertexBufferView.Size						= cube->GetVertexBuffer()->GetDescription().SizeInBytes;
			commandList->SetVertexBuffer(vertexBufferView, 0);

			Graphics::IndexBufferView indexBufferView = {};
			indexBufferView.BufferHandle			  = cube->GetIndexBuffer().get();
			indexBufferView.Offset					  = 0;
			indexBufferView.Size					  = cube->GetIndexBuffer()->GetDescription().SizeInBytes;
			indexBufferView.BufferFormat			  = Nexus::Graphics::IndexBufferFormat::UInt32;
			commandList->SetIndexBuffer(indexBufferView);

			auto indexCount = cube->GetIndexBuffer()->GetCount();
			commandList->DrawIndexed(indexCount, 0, 0);

			commandList->End();
			m_Device->SubmitCommandList(commandList);

			Ref<Texture2D>		   colourTexture = framebuffer->GetColorTexture(0);
			std::vector<unsigned char> pixels		 = colourTexture->GetData(0, 0, 0, size, size);
			cubemap->SetData(pixels.data(), face, 0, 0, 0, size, size);
		}

		return cubemap;
	}

	Ref<Texture2D> HdriProcessor::GetLoadedTexture() const
	{
		return m_HdriImage;
	}

	void HdriProcessor::GetDirection(CubemapFace face, float &yaw, float &pitch)
	{
		switch (face)
		{
			case CubemapFace::PositiveX:
				pitch = 0;
				yaw	  = 90;
				return;
			case CubemapFace::NegativeX:
				pitch = 0;
				yaw	  = -90;
				return;
			case CubemapFace::PositiveY:
				pitch = 90;
				yaw	  = 180;
				return;
			case CubemapFace::NegativeY:
				pitch = -90;
				yaw	  = 180;
				return;
			case CubemapFace::PositiveZ:
				pitch = 0;
				yaw	  = 180;
				return;
			case CubemapFace::NegativeZ:
				pitch = 0;
				yaw	  = 0;
				return;
			default: throw std::runtime_error("Failed to find a valid face");
		}
	}
}	 // namespace Nexus::Graphics