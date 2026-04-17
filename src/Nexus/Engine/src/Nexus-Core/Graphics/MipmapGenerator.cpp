#include "Nexus-Core/Graphics/MipmapGenerator.hpp"
#include "Nexus-Core/Runtime.hpp"
#include "Nexus-Core/Utils/GraphicsUtils.hpp"
#include "Nexus-Core/Utils/Utils.hpp"

const std::string c_MipmapVertexSource = "#version 450 core\n"
										 "layout (location = 0) in vec3 a_Position;\n"
										 "layout (location = 1) in vec2 a_TexCoord;\n"
										 "layout (location = 0) out vec2 o_TexCoord;\n"
										 "void main()\n"
										 "{\n"
										 "    gl_Position = vec4(a_Position, 1.0);\n"
										 "    o_TexCoord = a_TexCoord;\n"
										 "}";

const std::string c_MipmapFragmentSource = "#version 450 core\n"
										   "layout(location = 0) in vec2 a_TexCoord;\n"
										   "layout(location = 0) out vec4 o_Colour;\n"
										   "layout(binding = 0, set = 0) uniform sampler2D u_Texture;\n"
										   "void main()\n"
										   "{\n"
										   "    o_Colour = texture(u_Texture, a_TexCoord);\n"
										   "}";

namespace Nexus::Graphics
{
	MipmapGenerator::MipmapGenerator(IGraphicsDevice *device, Graphics::CommandQueueHandle commandQueue)
		: m_Device(device),
		  m_CommandQueue(commandQueue),
		  m_Quad(device, commandQueue, true)
	{
		m_CommandList = m_CommandQueue->CreateCommandList();

		ShaderModuleHandle m_VertexModule	= Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								   c_MipmapVertexSource,
																								   "Mipmap-Gen.vert",
																								   Nexus::GetApplication()->GetApplicationPath(),
																								   Nexus::Graphics::ShaderStage::Vertex);
		ShaderModuleHandle m_FragmentModule = Nexus::Utils::GetOrCreateCachedShaderFromSpirvSource(m_Device,
																								   c_MipmapFragmentSource,
																								   "Mipmap-Gen.frag",
																								   Nexus::GetApplication()->GetApplicationPath(),
																								   Nexus::Graphics::ShaderStage::Fragment);

		// set up pipeline for rendering
		Nexus::Graphics::GraphicsPipelineDescription pipelineDescription;
		pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

		pipelineDescription.VertexModule   = m_VertexModule;
		pipelineDescription.FragmentModule = m_FragmentModule;

		pipelineDescription.ColourFormats[0]  = PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourTargetCount = 1;
		pipelineDescription.DepthFormat		  = PixelFormat::D24_UNorm_S8_UInt;

		pipelineDescription.Layouts = {m_Quad.GetVertexBufferLayout()};

		pipelineDescription.ResourceDescription.Descriptors = {
			Nexus::Graphics::ResourceDescriptor {.Name				 = "u_Texture",
												 .Type				 = Nexus::Graphics::ResourceDescriptorType::CombinedImageSampler,
												 .CountOrSizeInBytes = 1}};

		m_Pipeline	  = m_Device->CreateGraphicsPipeline(pipelineDescription);
		m_ResourceSet = m_Device->CreateResourceSet(m_Pipeline);
	}

	std::vector<char> MipmapGenerator::GenerateMip(TextureHandle texture, uint32_t levelToGenerate, uint32_t levelToGenerateFrom, uint32_t arrayLayer)
	{
		std::vector<char> pixels = {};

		const uint32_t textureWidth	 = texture->GetDescription().Width;
		const uint32_t textureHeight = texture->GetDescription().Height;

		auto [mipWidth, mipHeight] = Utils::GetMipSize(textureWidth, textureHeight, levelToGenerate);

		// generate mip
		{
			Nexus::Graphics::FramebufferTextureCreateDescription framebufferTextureDesc = {};
			framebufferTextureDesc.ColourAttachmentFormats								= {texture->GetPixelFormat()};
			framebufferTextureDesc.Width												= mipWidth;
			framebufferTextureDesc.Height												= mipHeight;

			FramebufferHandle framebuffer = Utils::CreateFramebuffer(m_Device, framebufferTextureDesc);

			Nexus::Graphics::SamplerDescription samplerSpec;
			samplerSpec.MinimumLOD = levelToGenerateFrom;
			samplerSpec.MaximumLOD = levelToGenerateFrom;
			SamplerHandle sampler  = m_Device->CreateSampler(samplerSpec);

			Nexus::Graphics::TextureViewDescription viewDesc = {};
			viewDesc.TargetTexture							 = texture;
			viewDesc.Format									 = texture->GetPixelFormat();
			viewDesc.Range									 = {.BaseMipLevel	= 0,
																.LevelCount		= texture->GetMipLevels(),
																.BaseArrayLayer = 0,
																.LayerCount		= texture->GetDepthOrArrayLayers()};
			viewDesc.DebugName								 = "Mipmap Generator Texture View";
			TextureViewHandle textureView					 = m_Device->CreateTextureView(viewDesc);

			Nexus::Graphics::CombinedImageSampler ciSampler = {};
			ciSampler.ImageTexture							= textureView;
			ciSampler.ImageSampler							= sampler;

			m_ResourceSet->WriteCombinedImageSampler(ciSampler, "u_Texture");
			m_ResourceSet->Flush();

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = mipWidth;
			scissor.Height = mipHeight;

			Nexus::Graphics::Viewport viewport;
			viewport.X		  = 0;
			viewport.Y		  = 0;
			viewport.Width	  = mipWidth;
			viewport.Height	  = mipHeight;
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1;

			m_CommandList->Begin();
			m_CommandList->SetPipeline(m_Pipeline);
			m_CommandList->SetFramebuffer(framebuffer);
			m_CommandList->SetViewport(viewport);
			m_CommandList->SetScissor(scissor);

			Ref<IDeviceBuffer> vertexBuffer		= m_Quad.GetVertexBuffer();
			VertexBufferView   vertexBufferView = {};
			vertexBufferView.BufferHandle		= vertexBuffer;
			vertexBufferView.Offset				= 0;
			vertexBufferView.Size				= vertexBuffer->GetSizeInBytes();
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Ref<IDeviceBuffer> indexBuffer	   = m_Quad.GetIndexBuffer();
			IndexBufferView	   indexBufferView = {};
			indexBufferView.BufferHandle	   = indexBuffer;
			indexBufferView.Offset			   = 0;
			indexBufferView.Size			   = indexBuffer->GetSizeInBytes();
			indexBufferView.BufferFormat	   = IndexFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			Nexus::Graphics::ResourceSetBindingDescription resourceBindingDesc = {};
			resourceBindingDesc.TargetResourceSet							   = m_ResourceSet;
			resourceBindingDesc.DynamicOffsets								   = {};
			m_CommandList->SetResourceSet(resourceBindingDesc);

			DrawIndexedDescription drawDesc = {};
			drawDesc.VertexStart			= 0;
			drawDesc.IndexStart				= 0;
			drawDesc.InstanceStart			= 0;
			drawDesc.IndexCount				= 6;
			drawDesc.InstanceCount			= 1;
			m_CommandList->DrawIndexed(drawDesc);

			m_CommandList->End();

			m_CommandQueue->SubmitCommandList(m_CommandList);
			m_Device->WaitForIdle();

			TextureHandle framebufferTexture = framebuffer->GetColorTextureHandle(0);
			pixels							 = Utils::ReadFromTexture(m_CommandQueue, framebufferTexture, 0, 0, 0, 0, mipWidth, mipHeight);
		}

		return pixels;
	}

	uint32_t MipmapGenerator::GetMaximumNumberOfMips(uint32_t width, uint32_t height)
	{
		return std::floor(std::log2(std::max(width, height)));
	}
}	 // namespace Nexus::Graphics