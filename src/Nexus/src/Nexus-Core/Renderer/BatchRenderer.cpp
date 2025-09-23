#include "Nexus-Core/Renderer/BatchRenderer.hpp"

#include "Nexus-Core/Runtime.hpp"
#include "Nexus-Core/Utils/Utils.hpp"
#include "Nexus-Core/nxpch.hpp"

const std::string s_BatchVertexShaderSource = "#version 450 core\n"

											  "layout(location = 0) in vec4 Color;\n"
											  "layout(location = 1) in vec3 Position;\n"
											  "layout(location = 2) in float TexIndex;\n"
											  "layout(location = 3) in vec2 TexCoord;\n"
											  "layout(location = 4) in uvec2 EntityID;\n"

											  "layout(location = 0) out vec2 texCoord;\n"
											  "layout(location = 1) out vec4 outColor;\n"
											  "layout(location = 2) out flat float texIndex;\n"
											  "layout(location = 3) out flat uvec2 outEntityID;\n"

											  "layout(binding = 0, set = 0) uniform MVP\n"
											  "{\n"
											  "    mat4 u_MVP;\n"
											  "};\n"

											  "void main()\n"
											  "{\n"
											  "    gl_Position = u_MVP * vec4(Position, 1.0);\n"
											  "    texCoord = TexCoord;\n"
											  "    outColor = Color;\n"
											  "    texIndex = TexIndex;\n"
											  "    outEntityID = EntityID;\n"
											  "}";

const std::string s_BatchTextureFragmentShaderSource = "#version 450 core\n"

													   "layout (location = 0) out vec4 FragColor;\n"
													   "layout (location = 1) out uvec2 o_EntityID;\n"

													   "layout (location = 0) in vec2 texCoord;\n"
													   "layout (location = 1) in vec4 outColor;\n"
													   "layout (location = 2) in flat float texIndex;\n"
													   "layout(location = 3) in flat uvec2 outEntityID;\n"

													   "layout (set = 1, binding = 0) uniform sampler2D texture0;\n"
													   "layout (set = 1, binding = 1) uniform sampler2D texture1;\n"
													   "layout (set = 1, binding = 2) uniform sampler2D texture2;\n"
													   "layout (set = 1, binding = 3) uniform sampler2D texture3;\n"
													   "layout (set = 1, binding = 4) uniform sampler2D texture4;\n"
													   "layout (set = 1, binding = 5) uniform sampler2D texture5;\n"
													   "layout (set = 1, binding = 6) uniform sampler2D texture6;\n"
													   "layout (set = 1, binding = 7) uniform sampler2D texture7;\n"
													   "layout (set = 1, binding = 8) uniform sampler2D texture8;\n"
													   "layout (set = 1, binding = 9) uniform sampler2D texture9;\n"
													   "layout (set = 1, binding = 10) uniform sampler2D texture10;\n"
													   "layout (set = 1, binding = 11) uniform sampler2D texture11;\n"
													   "layout (set = 1, binding = 12) uniform sampler2D texture12;\n"
													   "layout (set = 1, binding = 13) uniform sampler2D texture13;\n"
													   "layout (set = 1, binding = 14) uniform sampler2D texture14;\n"
													   "layout (set = 1, binding = 15) uniform sampler2D texture15;\n"

													   "void main()\n"
													   "{\n"
													   "    switch (int(texIndex))\n"
													   "    {\n"
													   "        case 0: FragColor = texture(texture0, texCoord); break;\n"
													   "        case 1: FragColor = texture(texture1, texCoord); break;\n"
													   "        case 2: FragColor = texture(texture2, texCoord); break;\n"
													   "        case 3: FragColor = texture(texture3, texCoord); break;\n"
													   "        case 4: FragColor = texture(texture4, texCoord); break;\n"
													   "        case 5: FragColor = texture(texture5, texCoord); break;\n"
													   "        case 6: FragColor = texture(texture6, texCoord); break;\n"
													   "        case 7: FragColor = texture(texture7, texCoord); break;\n"
													   "        case 8: FragColor = texture(texture8, texCoord); break;\n"
													   "        case 9: FragColor = texture(texture9, texCoord); break;\n"
													   "        case 10: FragColor = texture(texture10, texCoord); break;\n"
													   "        case 11: FragColor = texture(texture11, texCoord); break;\n"
													   "        case 12: FragColor = texture(texture12, texCoord); break;\n"
													   "        case 13: FragColor = texture(texture13, texCoord); break;\n"
													   "        case 14: FragColor = texture(texture14, texCoord); break;\n"
													   "        case 15: FragColor = texture(texture15, texCoord); break;\n"
													   "    }\n"
													   "    FragColor *= outColor;\n"
													   "    o_EntityID = outEntityID;\n"
													   "	if (FragColor.a == 0) discard;\n"
													   "}\n";

const std::string s_BatchFontFragmentShaderSource = "#version 450 core\n"

													"layout (location = 0) out vec4 FragColor;\n"
													"layout (location = 1) out uvec2 o_EntityID;\n"

													"layout (location = 0) in vec2 texCoord;\n"
													"layout (location = 1) in vec4 outColor;\n"
													"layout (location = 2) in flat float texIndex;\n"
													"layout(location = 3) in flat uvec2 outEntityID;\n"

													"layout (set = 1, binding = 0) uniform sampler2D texture0;\n"
													"layout (set = 1, binding = 1) uniform sampler2D texture1;\n"
													"layout (set = 1, binding = 2) uniform sampler2D texture2;\n"
													"layout (set = 1, binding = 3) uniform sampler2D texture3;\n"
													"layout (set = 1, binding = 4) uniform sampler2D texture4;\n"
													"layout (set = 1, binding = 5) uniform sampler2D texture5;\n"
													"layout (set = 1, binding = 6) uniform sampler2D texture6;\n"
													"layout (set = 1, binding = 7) uniform sampler2D texture7;\n"
													"layout (set = 1, binding = 8) uniform sampler2D texture8;\n"
													"layout (set = 1, binding = 9) uniform sampler2D texture9;\n"
													"layout (set = 1, binding = 10) uniform sampler2D texture10;\n"
													"layout (set = 1, binding = 11) uniform sampler2D texture11;\n"
													"layout (set = 1, binding = 12) uniform sampler2D texture12;\n"
													"layout (set = 1, binding = 13) uniform sampler2D texture13;\n"
													"layout (set = 1, binding = 14) uniform sampler2D texture14;\n"
													"layout (set = 1, binding = 15) uniform sampler2D texture15;\n"

													"void main()\n"
													"{\n"
													"   float alpha = 0;"

													"    switch (int(texIndex))\n"
													"    {\n"
													"        case 0: alpha = texture(texture0, texCoord).r; break;\n"
													"        case 1: alpha = texture(texture1, texCoord).r; break;\n"
													"        case 2: alpha = texture(texture2, texCoord).r; break;\n"
													"        case 3: alpha = texture(texture3, texCoord).r; break;\n"
													"        case 4: alpha = texture(texture4, texCoord).r; break;\n"
													"        case 5: alpha = texture(texture5, texCoord).r; break;\n"
													"        case 6: alpha = texture(texture6, texCoord).r; break;\n"
													"        case 7: alpha = texture(texture7, texCoord).r; break;\n"
													"        case 8: alpha = texture(texture8, texCoord).r; break;\n"
													"        case 9: alpha = texture(texture9, texCoord).r; break;\n"
													"        case 10: alpha = texture(texture10, texCoord).r; break;\n"
													"        case 11: alpha = texture(texture11, texCoord).r; break;\n"
													"        case 12: alpha = texture(texture12, texCoord).r; break;\n"
													"        case 13: alpha = texture(texture13, texCoord).r; break;\n"
													"        case 14: alpha = texture(texture14, texCoord).r; break;\n"
													"        case 15: alpha = texture(texture15, texCoord).r; break;\n"
													"    }\n"
													"    FragColor = vec4(outColor.rgb, alpha);\n"
													"    o_EntityID = outEntityID;\n"
													"	 if (FragColor.a == 0) discard;\n"
													"}\n";

const std::string s_BatchSDFFragmentShaderSource = "#version 450 core\n"

												   "layout (location = 0) out vec4 FragColor;\n"
												   "layout (location = 1) out uvec2 o_EntityID;\n"

												   "layout (location = 0) in vec2 texCoord;\n"
												   "layout (location = 1) in vec4 outColor;\n"
												   "layout (location = 2) in flat float texIndex;\n"
												   "layout(location = 3) in flat uvec2 outEntityID;\n"

												   "layout (set = 1, binding = 0) uniform sampler2D texture0;\n"
												   "layout (set = 1, binding = 1) uniform sampler2D texture1;\n"
												   "layout (set = 1, binding = 2) uniform sampler2D texture2;\n"
												   "layout (set = 1, binding = 3) uniform sampler2D texture3;\n"
												   "layout (set = 1, binding = 4) uniform sampler2D texture4;\n"
												   "layout (set = 1, binding = 5) uniform sampler2D texture5;\n"
												   "layout (set = 1, binding = 6) uniform sampler2D texture6;\n"
												   "layout (set = 1, binding = 7) uniform sampler2D texture7;\n"
												   "layout (set = 1, binding = 8) uniform sampler2D texture8;\n"
												   "layout (set = 1, binding = 9) uniform sampler2D texture9;\n"
												   "layout (set = 1, binding = 10) uniform sampler2D texture10;\n"
												   "layout (set = 1, binding = 11) uniform sampler2D texture11;\n"
												   "layout (set = 1, binding = 12) uniform sampler2D texture12;\n"
												   "layout (set = 1, binding = 13) uniform sampler2D texture13;\n"
												   "layout (set = 1, binding = 14) uniform sampler2D texture14;\n"
												   "layout (set = 1, binding = 15) uniform sampler2D texture15;\n"

												   "void main()\n"
												   "{\n"
												   "    float alpha = 0;\n"

												   "    switch (int(texIndex))\n"
												   "    {\n"
												   "        case 0: alpha = texture(texture0, texCoord).r; break;\n"
												   "        case 1: alpha = texture(texture1, texCoord).r; break;\n"
												   "        case 2: alpha = texture(texture2, texCoord).r; break;\n"
												   "        case 3: alpha = texture(texture3, texCoord).r; break;\n"
												   "        case 4: alpha = texture(texture4, texCoord).r; break;\n"
												   "        case 5: alpha = texture(texture5, texCoord).r; break;\n"
												   "        case 6: alpha = texture(texture6, texCoord).r; break;\n"
												   "        case 7: alpha = texture(texture7, texCoord).r; break;\n"
												   "        case 8: alpha = texture(texture8, texCoord).r; break;\n"
												   "        case 9: alpha = texture(texture9, texCoord).r; break;\n"
												   "        case 10: alpha = texture(texture10, texCoord).r; break;\n"
												   "        case 11: alpha = texture(texture11, texCoord).r; break;\n"
												   "        case 12: alpha = texture(texture12, texCoord).r; break;\n"
												   "        case 13: alpha = texture(texture13, texCoord).r; break;\n"
												   "        case 14: alpha = texture(texture14, texCoord).r; break;\n"
												   "        case 15: alpha = texture(texture15, texCoord).r; break;\n"
												   "    }\n"

												   "    if (alpha < 0.45)\n"
												   "    {\n"
												   "        discard;\n"
												   "    }\n"

												   "    float w = fwidth(alpha);\n"
												   "    float opacity = smoothstep(0.5 - w, 0.5 + w, alpha);\n"

												   "    FragColor = vec4(outColor.rgb, opacity);\n"
												   "    o_EntityID = outEntityID;\n"
												   "}\n";

namespace Nexus::Graphics
{
	const uint32_t MAX_VERTEX_COUNT	 = 1024;
	const uint32_t MAX_TEXTURE_COUNT = 16;

	bool FindTextureInBatch(BatchInfo &info, Ref<Texture> texture, uint32_t &index)
	{
		for (uint32_t i = 0; i < info.Textures.size(); i++)
		{
			if (info.Textures[i] == texture)
			{
				index = i;
				return true;
			}
		}
		return false;
	}

	float GetOrCreateTexIndex(BatchInfo &info, Ref<Texture> texture)
	{
		uint32_t index = 0;
		if (FindTextureInBatch(info, texture, index))
		{
			return (float)index;
		}
		else
		{
			float texIndex = (float)info.Textures.size();
			info.Textures.push_back(texture);
			return texIndex;
		}
	}

	void FlushTextures(BatchInfo &info, Ref<Texture> blankTexture)
	{
		info.Textures.clear();
		info.Textures.push_back(blankTexture);
	}

	void ResetBatcher(BatchInfo &info, Ref<Texture> blankTexture)
	{
		info.Vertices.clear();
		info.Indices.clear();
		info.VertexCount = 0;
		info.IndexCount	 = 0;
		info.ShapeCount	 = 0;
		FlushTextures(info, blankTexture);
	}

	void CreateBatcher(BatchInfo								&info,
					   Nexus::Graphics::GraphicsDevice			*device,
					   Nexus::Ref<Nexus::Graphics::ShaderModule> vertexModule,
					   Nexus::Ref<Nexus::Graphics::ShaderModule> fragmentModule,
					   bool										 useDepthTest,
					   uint32_t									 sampleCount)
	{
		info.Vertices.resize(MAX_VERTEX_COUNT);
		info.Indices.resize(MAX_VERTEX_COUNT * 3);
		info.Textures.resize(MAX_TEXTURE_COUNT);

		Nexus::Graphics::GraphicsPipelineDescription description;
		description.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::CullNone;
		description.Layouts								 = {Nexus::Graphics::BatchVertex::GetLayout()};
		description.VertexModule						 = vertexModule;
		description.FragmentModule						 = fragmentModule;

		description.ColourFormats[0]  = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
		description.ColourFormats[1]  = Nexus::Graphics::PixelFormat::R32_G32_UInt;
		description.ColourTargetCount = 2;
		description.DepthFormat		  = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;

		description.ColourBlendStates[0].EnableBlending			= true;
		description.ColourBlendStates[0].SourceColourBlend		= BlendFactor::SourceAlpha;
		description.ColourBlendStates[0].DestinationColourBlend = BlendFactor::OneMinusSourceAlpha;
		description.ColourBlendStates[0].ColorBlendFunction		= BlendEquation::Add;
		description.ColourBlendStates[0].SourceAlphaBlend		= BlendFactor::SourceAlpha;
		description.ColourBlendStates[0].DestinationAlphaBlend	= BlendFactor::OneMinusSourceAlpha;
		description.ColourBlendStates[0].AlphaBlendFunction		= BlendEquation::Add;

		description.ColourBlendStates[1].EnableBlending = false;

		if (useDepthTest)
		{
			description.DepthStencilDesc.EnableDepthTest		 = true;
			description.DepthStencilDesc.EnableDepthWrite		 = true;
			description.DepthStencilDesc.DepthComparisonFunction = Nexus::Graphics::ComparisonFunction::Less;
		}
		else
		{
			description.DepthStencilDesc.EnableDepthTest  = false;
			description.DepthStencilDesc.EnableDepthWrite = false;
		}

		description.ColourTargetSampleCount = sampleCount;

		info.Pipeline	 = device->CreateGraphicsPipeline(description);
		info.ResourceSet = device->CreateResourceSet(info.Pipeline);

		Nexus::Graphics::DeviceBufferDescription vertexUploadDesc = {};
		vertexUploadDesc.Access									  = Graphics::BufferMemoryAccess::Upload;
		vertexUploadDesc.Usage									  = BUFFER_USAGE_NONE;
		vertexUploadDesc.StrideInBytes							  = sizeof(BatchVertex);
		vertexUploadDesc.SizeInBytes							  = info.Vertices.size() * sizeof(BatchVertex);
		info.VertexUploadBuffer									  = Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(vertexUploadDesc));

		Nexus::Graphics::DeviceBufferDescription vertexDesc = {};
		vertexDesc.Access									= Graphics::BufferMemoryAccess::Default;
		vertexDesc.Usage									= Graphics::BufferUsage::Vertex;
		vertexDesc.StrideInBytes							= sizeof(BatchVertex);
		vertexDesc.SizeInBytes								= info.Vertices.size() * sizeof(BatchVertex);
		info.VertexBuffer									= Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(vertexDesc));

		Nexus::Graphics::DeviceBufferDescription indexUploadDesc = {};
		indexUploadDesc.Access									 = Graphics::BufferMemoryAccess::Upload;
		indexUploadDesc.Usage									 = BUFFER_USAGE_NONE;
		indexUploadDesc.StrideInBytes							 = sizeof(uint32_t);
		indexUploadDesc.SizeInBytes								 = info.Indices.size() * sizeof(uint32_t);
		info.IndexUploadBuffer									 = Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(indexUploadDesc));

		Nexus::Graphics::DeviceBufferDescription indexDesc = {};
		indexDesc.Access								   = Graphics::BufferMemoryAccess::Default;
		indexDesc.Usage									   = Graphics::BufferUsage::Index;
		indexDesc.StrideInBytes							   = sizeof(uint32_t);
		indexDesc.SizeInBytes							   = info.Indices.size() * sizeof(uint32_t);
		info.IndexBuffer								   = Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(indexDesc));
	}

	BatchRenderer::BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Ref<ICommandQueue> commandQueue, bool useDepthTest, uint32_t sampleCount)
		: m_Device(device),
		  m_CommandQueue(commandQueue),
		  m_CommandList(m_Device->CreateCommandList()),
		  m_UseDepthTest(useDepthTest)
	{
		uint32_t textureData = 0xFFFFFFFF;

		Graphics::TextureDescription textureSpec = {};
		textureSpec.Width						 = 1;
		textureSpec.Height						 = 1;
		textureSpec.DepthOrArrayLayers			 = 1;
		textureSpec.Format						 = PixelFormat::R8_G8_B8_A8_UNorm;
		textureSpec.Usage						 = Graphics::TextureUsage_Sampled;
		m_BlankTexture							 = Ref<Texture>(m_Device->CreateTexture(textureSpec));
		m_Device->WriteToTexture(m_BlankTexture, m_CommandQueue, 0, 0, 0, 0, 0, 1, 1, &textureData, sizeof(textureData));

		Nexus::Ref<Nexus::Graphics::ShaderModule> vertexModule = device->GetOrCreateCachedShaderFromSpirvSource(s_BatchVertexShaderSource,
																												"Batch Renderer - Vertex Shader",
																												Nexus::Graphics::ShaderStage::Vertex);
		Nexus::Ref<Nexus::Graphics::ShaderModule> sdfFragmentModule =
			device->GetOrCreateCachedShaderFromSpirvSource(s_BatchSDFFragmentShaderSource,
														   "Batch Renderer - SDF Fragment Shader",
														   Nexus::Graphics::ShaderStage::Fragment);
		Nexus::Ref<Nexus::Graphics::ShaderModule> textureFragmentModule =
			device->GetOrCreateCachedShaderFromSpirvSource(s_BatchTextureFragmentShaderSource,
														   "Batch Renderer - Texture Fragment Shader",
														   Nexus::Graphics::ShaderStage::Fragment);
		Nexus::Ref<Nexus::Graphics::ShaderModule> fontFragmentModule =
			device->GetOrCreateCachedShaderFromSpirvSource(s_BatchFontFragmentShaderSource,
														   "Batch Renderer - Font Fragment Shader",
														   Nexus::Graphics::ShaderStage::Fragment);

		CreateBatcher(m_SDFBatchInfo, device, vertexModule, sdfFragmentModule, m_UseDepthTest, sampleCount);
		CreateBatcher(m_TextureBatchInfo, device, vertexModule, textureFragmentModule, m_UseDepthTest, sampleCount);
		CreateBatcher(m_FontBatchInfo, device, vertexModule, fontFragmentModule, m_UseDepthTest, sampleCount);

		Nexus::Graphics::DeviceBufferDescription uniformUploadDesc = {};
		uniformUploadDesc.Access								   = Graphics::BufferMemoryAccess::Upload;
		uniformUploadDesc.Usage									   = BUFFER_USAGE_NONE;
		uniformUploadDesc.StrideInBytes							   = sizeof(glm::mat4);
		uniformUploadDesc.SizeInBytes							   = sizeof(glm::mat4);
		m_UniformUploadBuffer									   = Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(uniformUploadDesc));

		Nexus::Graphics::DeviceBufferDescription uniformDesc = {};
		uniformDesc.Access									 = Graphics::BufferMemoryAccess::Default;
		uniformDesc.Usage									 = Graphics::BufferUsage::Uniform;
		uniformDesc.StrideInBytes							 = sizeof(glm::mat4);
		uniformDesc.SizeInBytes								 = sizeof(glm::mat4);
		m_UniformBuffer										 = Ref<Graphics::DeviceBuffer>(device->CreateDeviceBuffer(uniformDesc));

		Nexus::Graphics::SamplerDescription samplerSpec {};
		samplerSpec.SampleFilter = Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear;
		m_Sampler				 = m_Device->CreateSampler(samplerSpec);
	}

	void BatchRenderer::Resize()
	{
		m_Width	 = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().X;
		m_Height = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize().Y;
	}

	void BatchRenderer::Begin(Nexus::Graphics::RenderTarget target, Viewport viewport, Scissor scissor)
	{
		glm::mat4 projection = glm::ortho<float>(m_Viewport.X, m_Viewport.Width, m_Viewport.Height, m_Viewport.Y, -1.0f, 1.0f);
		Begin(target, viewport, scissor, projection);
	}

	void BatchRenderer::Begin(Nexus::Graphics::RenderTarget target, Viewport viewport, Scissor scissor, const glm::mat4 &camera)
	{
		if (m_IsStarted)
		{
			throw std::runtime_error("Batching has already started");
		}

		m_IsStarted	   = true;
		m_RenderTarget = target;

		ResetBatcher(m_TextureBatchInfo, m_BlankTexture);
		ResetBatcher(m_SDFBatchInfo, m_BlankTexture);
		ResetBatcher(m_FontBatchInfo, m_BlankTexture);

		m_Viewport		   = viewport;
		m_ScissorRectangle = scissor;

		m_UniformUploadBuffer->SetData(&camera, 0, sizeof(camera));

		Nexus::Graphics::BufferCopyDescription bufferCopy = {};
		bufferCopy.Source								  = m_UniformUploadBuffer;
		bufferCopy.Destination							  = m_UniformBuffer;
		bufferCopy.Copies								  = {{.ReadOffset = 0, .WriteOffset = 0, .Size = sizeof(camera)}};

		if (m_UniformUploadBuffer && m_UniformBuffer)
		{
			m_CommandList->Begin();
			m_CommandList->CopyBufferToBuffer(bufferCopy);
			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_Device->WaitForIdle();
		}
	}

	void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color)
	{
		DrawQuadFill(min, max, color, m_BlankTexture);
	}

	void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture> texture)
	{
		DrawQuadFill(min, max, color, texture, 1.0f);
	}

	void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture> texture, float tilingFactor)
	{
		const float texIndex = GetOrCreateTexIndex(m_TextureBatchInfo, texture);

		EnsureStarted();

		const uint32_t shapeVertexCount = 4;
		const uint32_t shapeIndexCount	= 6;

		EnsureSpace(m_TextureBatchInfo, shapeVertexCount, shapeIndexCount);

		glm::vec3 a(min.x, max.y, 0.0f);
		glm::vec3 b(max.x, max.y, 0.0f);
		glm::vec3 c(max.x, min.y, 0.0f);
		glm::vec3 d(min.x, min.y, 0.0f);

		m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(1 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(3 + m_TextureBatchInfo.VertexCount);

		BatchVertex v0;
		v0.Position	 = a;
		v0.TexCoords = {0.0f, 0.0f};
		v0.Color	 = color;
		v0.TexIndex	 = texIndex;
		m_TextureBatchInfo.Vertices.push_back(v0);

		BatchVertex v1;
		v1.Position	 = b;
		v1.TexCoords = {tilingFactor, 0.0f};
		v1.Color	 = color;
		v1.TexIndex	 = texIndex;
		m_TextureBatchInfo.Vertices.push_back(v1);

		BatchVertex v2;
		v2.Position	 = c;
		v2.TexCoords = {tilingFactor, tilingFactor};
		v2.Color	 = color;
		v2.TexIndex	 = texIndex;
		m_TextureBatchInfo.Vertices.push_back(v2);

		BatchVertex v3;
		v3.Position	 = d;
		v3.TexCoords = {0.0f, tilingFactor};
		v3.Color	 = color;
		v3.TexIndex	 = texIndex;
		m_TextureBatchInfo.Vertices.push_back(v3);

		m_TextureBatchInfo.IndexCount += shapeIndexCount;
		m_TextureBatchInfo.VertexCount += shapeVertexCount;
	}

	void BatchRenderer::DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color)
	{
		DrawQuadFill(rectangle, color, m_BlankTexture);
	}

	void BatchRenderer::DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture> texture)
	{
		DrawQuadFill(rectangle, color, texture, 1.0f);
	}

	void BatchRenderer::DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture> texture, float tilingFactor)
	{
		glm::vec2 min = {(float)rectangle.GetLeft(), (float)rectangle.GetTop()};
		glm::vec2 max = {(float)rectangle.GetRight(), (float)rectangle.GetBottom()};
		DrawQuadFill(min, max, color, texture, tilingFactor);
	}

	void BatchRenderer::DrawQuadFill(const glm::vec4 &color, Ref<Texture> texture, float tilingFactor, const glm::mat4 &transform, Nexus::GUID id)
	{
		if (!texture)
		{
			texture = m_BlankTexture;
		}

		const float texIndex = GetOrCreateTexIndex(m_TextureBatchInfo, texture);

		EnsureStarted();

		const uint32_t shapeVertexCount = 4;
		const uint32_t shapeIndexCount	= 6;

		EnsureSpace(m_TextureBatchInfo, shapeVertexCount, shapeIndexCount);

		std::array<glm::vec3, 4> quadVertices = {glm::vec3(-0.5f, 0.5f, 0.0f),
												 glm::vec3(0.5f, 0.5f, 0.0f),
												 glm::vec3(0.5f, -0.5f, 0.0f),
												 glm::vec3(-0.5f, -0.5f, 0.0f)};

		std::array<glm::vec3, 4> worldVertices;

		for (size_t i = 0; i < quadVertices.size(); i++) { worldVertices[i] = transform * glm::vec4(quadVertices[i], 1.0f); }

		m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(1 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(3 + m_TextureBatchInfo.VertexCount);

		std::pair<uint32_t, uint32_t> outId	   = id.Split();
		Point2D<uint32_t>			  entityId = {outId.first, outId.second};

		float id1 = (float)entityId.X;
		float id2 = (float)entityId.Y;

		BatchVertex v0;
		v0.Position	 = worldVertices[0];
		v0.TexCoords = {0.0f, 0.0f};
		v0.Color	 = color;
		v0.TexIndex	 = texIndex;
		v0.Id		 = entityId;
		m_TextureBatchInfo.Vertices.push_back(v0);

		BatchVertex v1;
		v1.Position	 = worldVertices[1];
		v1.TexCoords = {tilingFactor, 0.0f};
		v1.Color	 = color;
		v1.TexIndex	 = texIndex;
		v1.Id		 = entityId;
		m_TextureBatchInfo.Vertices.push_back(v1);

		BatchVertex v2;
		v2.Position	 = worldVertices[2];
		v2.TexCoords = {tilingFactor, tilingFactor};
		v2.Color	 = color;
		v2.TexIndex	 = texIndex;
		v2.Id		 = entityId;
		m_TextureBatchInfo.Vertices.push_back(v2);

		BatchVertex v3;
		v3.Position	 = worldVertices[3];
		v3.TexCoords = {0.0f, tilingFactor};
		v3.Color	 = color;
		v3.TexIndex	 = texIndex;
		v3.Id		 = entityId;
		m_TextureBatchInfo.Vertices.push_back(v3);

		m_TextureBatchInfo.IndexCount += shapeIndexCount;
		m_TextureBatchInfo.VertexCount += shapeVertexCount;
	}

	void BatchRenderer::DrawQuad(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, float thickness)
	{
		DrawLine({min.x, max.y}, {max.x, max.y}, color, thickness);
		DrawLine({max.x, max.y}, {max.x, min.y}, color, thickness);
		DrawLine({max.x, min.y}, {min.x, min.y}, color, thickness);
		DrawLine({min.x, min.y}, {min.x, max.y}, color, thickness);
	}

	void BatchRenderer::DrawQuad(const Rectangle<float> &rectangle, const glm::vec4 &color, float thickness)
	{
		glm::vec2 min = {(float)rectangle.GetLeft(), (float)rectangle.GetTop()};
		glm::vec2 max = {(float)rectangle.GetRight(), (float)rectangle.GetBottom()};
		DrawQuad(min, max, color, thickness);
	}

	void BatchRenderer::DrawCharacter(char character, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, Font *font)
	{
		EnsureStarted();

		const uint32_t shapeVertexCount = 4;
		const uint32_t shapeIndexCount	= 6;

		Nexus::Graphics::BatchInfo *info = &m_FontBatchInfo;

		// if we are using a SDF font, then we need to use an SDF pipeline
		// otherwise just use a regular rasterized font pipeline
		if (font->GetFontType() == Nexus::Graphics::FontType::SDF)
		{
			info = &m_SDFBatchInfo;
		}

		EnsureSpace(*info, shapeVertexCount, shapeIndexCount);

		const auto &characterInfo = font->GetCharacter(character);
		glm::vec2	min			  = position;
		glm::vec2	max			  = {position.x + size.x, position.y + size.y};

		glm::vec3 a(min.x, max.y, 0.0f);
		glm::vec3 b(max.x, max.y, 0.0f);
		glm::vec3 c(max.x, min.y, 0.0f);
		glm::vec3 d(min.x, min.y, 0.0f);

		info->Indices.push_back(0 + info->VertexCount);
		info->Indices.push_back(1 + info->VertexCount);
		info->Indices.push_back(2 + info->VertexCount);
		info->Indices.push_back(0 + info->VertexCount);
		info->Indices.push_back(2 + info->VertexCount);
		info->Indices.push_back(3 + info->VertexCount);

		float texIndex = GetOrCreateTexIndex(*info, font->GetTexture());

		BatchVertex v0;
		v0.Position	 = a;
		v0.TexCoords = {characterInfo.TexCoordsMin.x, characterInfo.TexCoordsMin.y};
		v0.Color	 = color;
		v0.TexIndex	 = texIndex;
		info->Vertices.push_back(v0);

		BatchVertex v1;
		v1.Position	 = b;
		v1.TexCoords = {characterInfo.TexCoordsMax.x, characterInfo.TexCoordsMin.y};
		v1.Color	 = color;
		v1.TexIndex	 = texIndex;
		info->Vertices.push_back(v1);

		BatchVertex v2;
		v2.Position	 = c;
		v2.TexCoords = {characterInfo.TexCoordsMax.x, characterInfo.TexCoordsMax.y};
		v2.Color	 = color;
		v2.TexIndex	 = texIndex;
		info->Vertices.push_back(v2);

		BatchVertex v3;
		v3.Position	 = d;
		v3.TexCoords = {characterInfo.TexCoordsMin.x, characterInfo.TexCoordsMax.y};
		v3.Color	 = color;
		v3.TexIndex	 = texIndex;
		info->Vertices.push_back(v3);

		info->IndexCount += shapeIndexCount;
		info->VertexCount += shapeVertexCount;
	}

	void BatchRenderer::DrawString(const std::string &text, const glm::vec2 &position, uint32_t size, const glm::vec4 &color, Font *font)
	{
		// because all fonts we use are SDFs, we need to calculate how big the
		// generated quads should be from the original size of the glyph
		float scale = 1.0f / font->GetSize() * size;

		// initial text position
		float worldX = position.x;
		float worldY = position.y + ((font->GetLineHeight() * scale) * 0.75f);

		for (const char c : text)
		{
			Character ch = font->GetCharacter(c);

			float xpos = worldX + (ch.Bearing.x * scale);
			float ypos = worldY - (ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			if (c == '\n')
			{
				xpos = position.x;
				worldY += font->GetLineHeight() * scale;
			}
			else if (c == '\t')
			{
				const auto &spaceInfo = font->GetCharacter(' ');
				xpos += ((spaceInfo.Advance >> 6) * scale) * 4;
			}
			else if (c == '\r')
			{
				continue;
			}
			else
			{
				// render glyph
				DrawCharacter(c, {xpos, ypos}, {w, h}, color, font);

				// remember to increase the x offset of the next character
				xpos += ((ch.Advance >> 6)) * scale;
			}

			worldX = xpos;
		}
	}

	void BatchRenderer::DrawLine(const glm::vec2 &a, const glm::vec2 &b, const glm::vec4 &color, float thickness)
	{
		EnsureStarted();

		const uint32_t shapeVertexCount = 4;
		const uint32_t shapeIndexCount	= 6;

		EnsureSpace(m_TextureBatchInfo, shapeVertexCount, shapeIndexCount);

		thickness = glm::clamp(thickness, 1.0f, 10.0f);

		glm::vec2 e1 = b - a;
		e1			 = glm::normalize(e1);
		e1 *= thickness;

		glm::vec2 e2 = -e1;

		glm::vec2 n1 = {-e1.y, e1.x};
		glm::vec2 n2 = -n1;

		glm::vec2 q1 = a + n1 + e2;
		glm::vec2 q2 = b + n1 + e1;
		glm::vec2 q3 = b + n2 + e1;
		glm::vec2 q4 = a + n2 + e2;

		m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(1 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(3 + m_TextureBatchInfo.VertexCount);

		BatchVertex v0;
		v0.Position	 = glm::vec3(q1, 0.0f);
		v0.TexCoords = {0.0f, 0.0f};
		v0.Color	 = color;
		v0.TexIndex	 = 0;
		m_TextureBatchInfo.Vertices.push_back(v0);

		BatchVertex v1;
		v1.Position	 = glm::vec3(q2, 0.0f);
		v1.TexCoords = {1.0f, 0.0f};
		v1.Color	 = color;
		v1.TexIndex	 = 0;
		m_TextureBatchInfo.Vertices.push_back(v1);

		BatchVertex v2;
		v2.Position	 = glm::vec3(q3, 0.0f);
		v2.TexCoords = {1.0f, 1.0f};
		v2.Color	 = color;
		v2.TexIndex	 = 0;
		m_TextureBatchInfo.Vertices.push_back(v2);

		BatchVertex v3;
		v3.Position	 = glm::vec3(q4, 0.0f);
		v3.TexCoords = {0.0f, 1.0f};
		v3.Color	 = color;
		v3.TexIndex	 = 0;
		m_TextureBatchInfo.Vertices.push_back(v3);

		m_TextureBatchInfo.IndexCount += shapeIndexCount;
		m_TextureBatchInfo.VertexCount += shapeVertexCount;
	}

	void BatchRenderer::DrawCircle(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float thickness)
	{
		const uint32_t minPoints = 3;
		const uint32_t maxPoints = 256;

		numberOfPoints = glm::clamp<float>(numberOfPoints, minPoints, maxPoints);

		// float deltaAngle = glm::radians(180.0f) / (float)numberOfPoints;

		float deltaAngle = glm::two_pi<float>() / (float)numberOfPoints;
		float angle		 = glm::radians(180.0f);

		for (int i = 0; i < numberOfPoints; i++)
		{
			float ax = glm::sin(angle) * radius + (position.x);
			float ay = glm::cos(angle) * radius + (position.y);

			angle += deltaAngle;

			float bx = glm::sin(angle) * radius + (position.x);
			float by = glm::cos(angle) * radius + (position.y);

			DrawLine({ax, ay}, {bx, by}, color, thickness);
		}
	}

	void BatchRenderer::DrawCircle(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, float thickness)
	{
		const auto &circlePos = circle.GetPosition();
		DrawCircle({circlePos.X, circlePos.Y}, circle.GetRadius(), color, numberOfPoints, thickness);
	}

	void BatchRenderer::DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints)
	{
		DrawCircleFill(position, radius, color, numberOfPoints, m_BlankTexture);
	}

	void BatchRenderer::DrawCircleRegionFill(const glm::vec2 &position,
											 float			  radius,
											 const glm::vec4 &color,
											 uint32_t		  numberOfPoints,
											 float			  startAngle,
											 float			  fillAngle)
	{
		DrawCircleRegionFill(position, radius, color, numberOfPoints, startAngle, fillAngle, m_BlankTexture);
	}

	void BatchRenderer::DrawCircleRegionFill(const glm::vec2 &position,
											 float			  radius,
											 const glm::vec4 &color,
											 uint32_t		  numberOfPoints,
											 float			  startAngle,
											 float			  fillAngle,
											 Ref<Texture>	  texture)
	{
		DrawCircleRegionFill(position, radius, color, numberOfPoints, startAngle, fillAngle, texture, 1.0f);
	}

	void BatchRenderer::DrawCircleRegionFill(const glm::vec2 &position,
											 float			  radius,
											 const glm::vec4 &color,
											 uint32_t		  numberOfPoints,
											 float			  startAngle,
											 float			  fillAngle,
											 Ref<Texture>	  texture,
											 float			  tilingFactor)
	{
		const uint32_t minPoints = 3;
		const uint32_t maxPoints = 256;

		numberOfPoints	   = glm::clamp<float>(numberOfPoints, minPoints, maxPoints);
		float deltaAngle   = glm::radians(fillAngle) / (float)numberOfPoints;
		float currentAngle = glm::radians(startAngle) + glm::radians(180.0f);	 // start rendering the circle from the centre top

		const glm::vec2 topLeft		= {position.x - radius, position.y - radius};
		const glm::vec2 bottomRight = {position.x + radius, position.y + radius};

		const glm::vec3 centre = {position.x, position.y, 0.0f};
		const glm::vec2 uvTL   = {0.0f, 0.0f};
		const glm::vec2 uvBR   = {tilingFactor, tilingFactor};

		glm::vec2 centreUV = {Nexus::Utils::ReMapRange(topLeft.x, bottomRight.x, uvTL.x, uvBR.x, centre.x),
							  Nexus::Utils::ReMapRange(topLeft.y, bottomRight.y, uvTL.y, uvBR.y, centre.y)};

		for (int i = 0; i < numberOfPoints; i++)
		{
			glm::vec3 posA = {glm::sin(currentAngle) * radius + (position.x), glm::cos(currentAngle) * radius + (position.y), 0.0f};

			glm::vec2 uvA = {Nexus::Utils::ReMapRange(topLeft.x, bottomRight.x, uvTL.x, uvBR.x, posA.x),
							 Nexus::Utils::ReMapRange(topLeft.y, bottomRight.y, uvTL.y, uvBR.y, posA.y)};

			currentAngle -= deltaAngle;

			glm::vec3 posB = {glm::sin(currentAngle) * radius + (position.x), glm::cos(currentAngle) * radius + (position.y), 0.0f};

			glm::vec2 uvB = {Nexus::Utils::ReMapRange(topLeft.x, bottomRight.x, uvTL.x, uvBR.x, posB.x),
							 Nexus::Utils::ReMapRange(topLeft.y, bottomRight.y, uvTL.y, uvBR.y, posB.y)};

			DrawTriangle(centre, centreUV, posA, uvA, posB, uvB, color, texture);
		}
	}

	void BatchRenderer::DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture> texture)
	{
		DrawCircleRegionFill(position, radius, color, numberOfPoints, 0.0f, 360.0f, texture);
	}

	void BatchRenderer::DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints)
	{
		DrawCircleFill(circle, color, numberOfPoints, m_BlankTexture);
	}

	void BatchRenderer::DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture> texture)
	{
		const auto &pos = circle.GetPosition();
		DrawCircleFill({pos.X, pos.Y}, circle.GetRadius(), color, numberOfPoints, texture);
	}

	void BatchRenderer::DrawCircleFill(const Circle<float> &circle,
									   const glm::vec4	   &color,
									   uint32_t				numberOfPoints,
									   Ref<Texture>			texture,
									   float				tilingFactor)
	{
		DrawCircleRegionFill({circle.GetPosition().X, circle.GetPosition().Y},
							 circle.GetRadius(),
							 color,
							 numberOfPoints,
							 0.0f,
							 360.0f,
							 texture,
							 tilingFactor);
	}

	void BatchRenderer::DrawCross(const Rectangle<float> &rectangle, float thickness, const glm::vec4 &color)
	{
		glm::vec2 topLeft	  = {rectangle.GetLeft() + thickness, rectangle.GetTop() + thickness};
		glm::vec2 bottomRight = {rectangle.GetRight() - thickness, rectangle.GetBottom() - thickness};
		glm::vec2 bottomLeft  = {rectangle.GetLeft() + thickness, rectangle.GetBottom() - thickness};
		glm::vec2 topRight	  = {rectangle.GetRight() - thickness, rectangle.GetTop() + thickness};

		DrawLine(topLeft, bottomRight, color, thickness);
		DrawLine(bottomLeft, topRight, color, thickness);
	}

	void BatchRenderer::DrawTriangle(const glm::vec3 &pos0,
									 const glm::vec2 &uv0,
									 const glm::vec3 &pos1,
									 const glm::vec2 &uv1,
									 const glm::vec3 &pos2,
									 const glm::vec2 &uv2,
									 const glm::vec4 &color)
	{
		DrawTriangle(pos0, uv0, pos1, uv1, pos2, uv2, color, m_BlankTexture);
	}

	void BatchRenderer::DrawTriangle(const glm::vec3 &pos0,
									 const glm::vec2 &uv0,
									 const glm::vec3 &pos1,
									 const glm::vec2 &uv1,
									 const glm::vec3 &pos2,
									 const glm::vec2 &uv2,
									 const glm::vec4 &color,
									 Ref<Texture>	  texture)
	{
		float texIndex = GetOrCreateTexIndex(m_TextureBatchInfo, texture);

		EnsureStarted();

		const uint32_t shapeVertexCount = 3;
		const uint32_t shapeIndexCount	= 3;

		EnsureSpace(m_TextureBatchInfo, shapeVertexCount, shapeIndexCount);

		m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(1 + m_TextureBatchInfo.VertexCount);
		m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);

		BatchVertex v0;
		v0.Position	 = pos0;
		v0.TexCoords = uv0;
		v0.Color	 = color;
		v0.TexIndex	 = texIndex;
		m_TextureBatchInfo.Vertices.push_back(v0);

		BatchVertex v1;
		v1.Position	 = pos1;
		v1.TexCoords = uv1;
		v1.Color	 = color;
		v1.TexIndex	 = texIndex;
		m_TextureBatchInfo.Vertices.push_back(v1);

		BatchVertex v2;
		v2.Position	 = pos2;
		v2.TexCoords = uv2;
		v2.Color	 = color;
		v2.TexIndex	 = texIndex;
		m_TextureBatchInfo.Vertices.push_back(v2);

		m_TextureBatchInfo.IndexCount += shapeIndexCount;
		m_TextureBatchInfo.VertexCount += shapeVertexCount;
	}

	void BatchRenderer::DrawTriangle(const Graphics::Triangle2D &tri, const glm::vec4 &color)
	{
		DrawTriangle({tri.A.x, tri.A.y, 0.0f}, {0.0f, 0.0f}, {tri.B.x, tri.B.y, 0.0f}, {0.0f, 0.0f}, {tri.C.x, tri.C.y, 0.0f}, {0.0f, 0.0f}, color);
	}

	void BatchRenderer::DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color)
	{
		DrawPolygonFill(polygon, color, m_BlankTexture);
	}

	void BatchRenderer::DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture> texture)
	{
		DrawPolygonFill(polygon, color, m_BlankTexture, 1.0f);
	}

	void BatchRenderer::DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture> texture, float tilingFactor)
	{
		const auto					  &boundingRectangle = polygon.GetBoundingRectangle();
		const std::vector<Triangle2D> &tris				 = polygon.GetTriangles();

		const glm::vec2 uvTL = {0.0f, 0.0f};
		const glm::vec2 uvBR = {tilingFactor, tilingFactor};

		for (const auto &triangle2D : tris)
		{
			const Triangle3D triToDraw = triangle2D;

			const glm::vec2 uvA = {
				Nexus::Utils::ReMapRange(boundingRectangle.GetLeft(), boundingRectangle.GetRight(), uvTL.x, uvBR.x, triToDraw.A.x),
				Nexus::Utils::ReMapRange(boundingRectangle.GetTop(), boundingRectangle.GetBottom(), uvTL.y, uvBR.y, triToDraw.A.y)};

			const glm::vec2 uvB = {
				Nexus::Utils::ReMapRange(boundingRectangle.GetLeft(), boundingRectangle.GetRight(), uvTL.x, uvBR.x, triToDraw.B.x),
				Nexus::Utils::ReMapRange(boundingRectangle.GetTop(), boundingRectangle.GetBottom(), uvTL.y, uvBR.y, triToDraw.B.y)};

			const glm::vec2 uvC = {
				Nexus::Utils::ReMapRange(boundingRectangle.GetLeft(), boundingRectangle.GetRight(), uvTL.x, uvBR.x, triToDraw.C.x),
				Nexus::Utils::ReMapRange(boundingRectangle.GetTop(), boundingRectangle.GetBottom(), uvTL.y, uvBR.y, triToDraw.C.y)};

			DrawTriangle(triToDraw.A, uvA, triToDraw.B, uvB, triToDraw.C, uvC, color, texture);
		}
	}

	void BatchRenderer::DrawRoundedRectangle(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, float thickness)
	{
		const std::vector<glm::vec2> points = roundedRectangle.CreateOutline();

		for (size_t i = 0; i < points.size(); i++)
		{
			glm::vec2 p0 = points[i];
			glm::vec2 p1 = points[(i + 1) % points.size()];
			DrawLine(p0, p1, color, thickness);
		}
	}

	void BatchRenderer::DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color)
	{
		DrawRoundedRectangleFill(roundedRectangle, color, m_BlankTexture);
	}

	void BatchRenderer::DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, Ref<Texture> texture)
	{
		DrawRoundedRectangleFill(roundedRectangle, color, texture, 1.0f);
	}

	void BatchRenderer::DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle,
												 const glm::vec4		&color,
												 Ref<Texture>			 texture,
												 float					 tilingFactor)
	{
		const Polygon &poly = roundedRectangle.CreatePolygon();
		DrawPolygonFill(poly, color, texture, tilingFactor);
	}

	void BatchRenderer::End()
	{
		EnsureStarted();
		Flush();

		m_IsStarted = false;
	}

	void BatchRenderer::Flush()
	{
		EnsureStarted();
		PerformDraw(m_TextureBatchInfo);
		PerformDraw(m_SDFBatchInfo);
		PerformDraw(m_FontBatchInfo);
	}

	void BatchRenderer::EnsureStarted()
	{
		if (!m_IsStarted)
		{
			throw std::runtime_error("Batching was never started");
		}
	}

	void BatchRenderer::EnsureSpace(BatchInfo &info, uint32_t shapeVertexCount, uint32_t shapeIndexCount)
	{
		uint32_t maxVertexCount = info.Vertices.capacity();
		uint32_t maxIndexCount	= info.Indices.capacity();

		if (shapeVertexCount > maxVertexCount || shapeIndexCount > maxIndexCount)
		{
			throw std::runtime_error("Max vertex or index count reached for one draw call");
		}

		if (info.VertexCount + shapeVertexCount > maxVertexCount || info.IndexCount + shapeIndexCount > maxIndexCount ||
			info.Textures.size() > MAX_TEXTURE_COUNT)
		{
			Flush();
		}
	}

	void BatchRenderer::PerformDraw(BatchInfo &info)
	{
		if (info.Vertices.size() == 0 || info.Indices.size() == 0)
		{
			return;
		}

		info.VertexUploadBuffer->SetData(info.Vertices.data(), 0, info.Vertices.size() * sizeof(info.Vertices[0]));
		info.IndexUploadBuffer->SetData(info.Indices.data(), 0, info.Indices.size() * sizeof(info.Indices[0]));

		UniformBufferView uniformBufferView = {};
		uniformBufferView.BufferHandle		= m_UniformBuffer;
		uniformBufferView.Offset			= 0;
		uniformBufferView.Size				= m_UniformBuffer->GetDescription().SizeInBytes;
		info.ResourceSet->WriteUniformBuffer(uniformBufferView, "MVP");

		for (uint32_t i = 0; i < MAX_TEXTURE_COUNT; i++)
		{
			std::string textureName = "texture" + std::to_string(i);
			if (i < info.Textures.size())
			{
				info.ResourceSet->WriteCombinedImageSampler(info.Textures.at(i), m_Sampler, textureName);
			}
			else
			{
				info.ResourceSet->WriteCombinedImageSampler(m_BlankTexture, m_Sampler, textureName);
			}
		}

		m_CommandList->Begin();

		// upload vertex data
		{
			BufferCopyDescription bufferCopy = {};
			bufferCopy.Source				 = info.VertexUploadBuffer;
			bufferCopy.Destination			 = info.VertexBuffer;
			bufferCopy.Copies				 = {{.ReadOffset = 0, .WriteOffset = 0, .Size = info.Vertices.size() * sizeof(info.Vertices[0])}};

			m_CommandList->CopyBufferToBuffer(bufferCopy);
		}

		// upload index data
		// upload index data
		{
			BufferCopyDescription bufferCopy = {};
			bufferCopy.Source				 = info.IndexUploadBuffer;
			bufferCopy.Destination			 = info.IndexBuffer;
			bufferCopy.Copies				 = {{.ReadOffset = 0, .WriteOffset = 0, .Size = info.Indices.size() * sizeof(info.Indices[0])}};

			m_CommandList->CopyBufferToBuffer(bufferCopy);
		}

		m_CommandList->SetPipeline(info.Pipeline);
		m_CommandList->SetRenderTarget(m_RenderTarget);
		m_CommandList->SetViewport(m_Viewport);
		m_CommandList->SetScissor(m_ScissorRectangle);
		m_CommandList->SetResourceSet(info.ResourceSet);

		VertexBufferView vertexBufferView = {};
		vertexBufferView.BufferHandle	  = info.VertexBuffer;
		vertexBufferView.Offset			  = 0;
		vertexBufferView.Size			  = info.VertexBuffer->GetSizeInBytes();
		m_CommandList->SetVertexBuffer(vertexBufferView, 0);

		IndexBufferView indexBufferView = {};
		indexBufferView.BufferHandle	= info.IndexBuffer;
		indexBufferView.Offset			= 0;
		indexBufferView.Size			= info.IndexBuffer->GetSizeInBytes();
		indexBufferView.BufferFormat	= Graphics::IndexFormat::UInt32;
		m_CommandList->SetIndexBuffer(indexBufferView);

		DrawIndexedDescription drawDesc = {};
		drawDesc.VertexStart			= 0;
		drawDesc.IndexStart				= 0;
		drawDesc.InstanceStart			= 0;
		drawDesc.IndexCount				= info.IndexCount;
		drawDesc.InstanceCount			= 1;
		m_CommandList->DrawIndexed(drawDesc);

		m_CommandList->End();
		m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);

		FlushTextures(info, m_BlankTexture);
	}
}	 // namespace Nexus::Graphics