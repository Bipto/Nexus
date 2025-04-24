#include "Nexus-Core/Graphics/MipmapGenerator.hpp"

#include "Nexus-Core/nxpch.hpp"

const std::string c_MipmapVertexSource = "#version 450 core\n"
										 "layout (location = 0) in vec3 Position;\n"
										 "layout (location = 1) in vec2 TexCoord;\n"
										 "layout (location = 0) out vec2 OutTexCoord;\n"
										 "void main()\n"
										 "{\n"
										 "    gl_Position = vec4(Position, 1.0);\n"
										 "    OutTexCoord = TexCoord;\n"
										 "}";

const std::string c_MipmapFragmentSource = "#version 450 core\n"
										   "layout(location = 0) in vec2 OutTexCoord;\n"
										   "layout(location = 0) out vec4 FragColor;\n"
										   "layout(binding = 0, set = 0) uniform sampler2D texSampler;\n"
										   "void main()\n"
										   "{\n"
										   "    FragColor = texture(texSampler, OutTexCoord);\n"
										   "}";

namespace Nexus::Graphics
{
	MipmapGenerator::MipmapGenerator(GraphicsDevice *device) : m_Device(device), m_Quad(device, true)
	{
		m_CommandList = m_Device->CreateCommandList();

		Ref<ShaderModule> m_VertexModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_MipmapVertexSource, "Mipmap-Gen.vert", Nexus::Graphics::ShaderStage::Vertex);
		Ref<ShaderModule> m_FragmentModule =
			m_Device->GetOrCreateCachedShaderFromSpirvSource(c_MipmapFragmentSource, "Mipmap-Gen.frag", Nexus::Graphics::ShaderStage::Fragment);

		// set up pipeline for rendering
		Nexus::Graphics::PipelineDescription pipelineDescription;
		pipelineDescription.RasterizerStateDesc.TriangleCullMode  = Nexus::Graphics::CullMode::CullNone;
		pipelineDescription.RasterizerStateDesc.TriangleFrontFace = Nexus::Graphics::FrontFace::CounterClockwise;

		pipelineDescription.VertexModule   = m_VertexModule;
		pipelineDescription.FragmentModule = m_FragmentModule;

		pipelineDescription.ColourFormats[0]  = PixelFormat::R8_G8_B8_A8_UNorm;
		pipelineDescription.ColourTargetCount = 1;
		pipelineDescription.DepthFormat		  = PixelFormat::D24_UNorm_S8_UInt;

		pipelineDescription.ResourceSetSpec.SampledImages = {{"texSampler", 0, 0}};

		pipelineDescription.Layouts = {m_Quad.GetVertexBufferLayout()};
		m_Pipeline					= m_Device->CreatePipeline(pipelineDescription);
		m_ResourceSet				= m_Device->CreateResourceSet(m_Pipeline);
	}

	std::vector<unsigned char> MipmapGenerator::GenerateMip(Ref<Texture2D> texture, uint32_t levelToGenerate, uint32_t levelToGenerateFrom)
	{
		std::vector<unsigned char> pixels = {};

		const uint32_t textureWidth	 = texture->GetSpecification().Width;
		const uint32_t textureHeight = texture->GetSpecification().Height;

		auto [mipWidth, mipHeight] = Utils::GetMipSize(textureWidth, textureHeight, levelToGenerate);

		// generate mip
		{
			Nexus::Graphics::FramebufferSpecification framebufferSpec;
			framebufferSpec.ColorAttachmentSpecification = {texture->GetSpecification().Format};
			framebufferSpec.Width						 = mipWidth;
			framebufferSpec.Height						 = mipHeight;
			framebufferSpec.Samples						 = texture->GetSpecification().Samples;

			Ref<Framebuffer> framebuffer = m_Device->CreateFramebuffer(framebufferSpec);

			Nexus::Graphics::SamplerSpecification samplerSpec;
			samplerSpec.MinimumLOD = levelToGenerateFrom;
			samplerSpec.MaximumLOD = levelToGenerateFrom;
			Ref<Sampler> sampler   = m_Device->CreateSampler(samplerSpec);

			Ref<Texture2D> framebufferTexture = framebuffer->GetColorTexture(0);

			m_ResourceSet->WriteCombinedImageSampler(texture, sampler, "texSampler");

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
			m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(framebuffer));
			m_CommandList->SetViewport(viewport);
			m_CommandList->SetScissor(scissor);

			Ref<DeviceBuffer> vertexBuffer	   = m_Quad.GetVertexBuffer();
			VertexBufferView  vertexBufferView = {};
			vertexBufferView.BufferHandle	   = vertexBuffer.get();
			vertexBufferView.Offset			   = 0;
			m_CommandList->SetVertexBuffer(vertexBufferView, 0);

			Ref<DeviceBuffer> indexBuffer	  = m_Quad.GetIndexBuffer();
			IndexBufferView	  indexBufferView = {};
			indexBufferView.BufferHandle	  = indexBuffer.get();
			indexBufferView.Offset			  = 0;
			indexBufferView.BufferFormat	  = IndexBufferFormat::UInt32;
			m_CommandList->SetIndexBuffer(indexBufferView);

			m_CommandList->SetResourceSet(m_ResourceSet);
			m_CommandList->DrawIndexed(6, 0, 0);
			m_CommandList->End();
			m_Device->SubmitCommandList(m_CommandList);

			pixels = framebufferTexture->GetData(0, 0, 0, mipWidth, mipHeight);
		}

		return pixels;
	}

	uint32_t MipmapGenerator::GetMaximumNumberOfMips(uint32_t width, uint32_t height)
	{
		return std::floor(std::log2(std::max(width, height)));
	}
}	 // namespace Nexus::Graphics