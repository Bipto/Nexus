#include "BatchRenderer.hpp"

#include "Nexus-Core/Utils/Utils.hpp"
#include "Nexus-Core/nxpch.hpp"

const std::string s_BatchVertexShaderSource = "#version 450 core\n"

                                              "layout(location = 0) in vec3 Position;\n"
                                              "layout(location = 1) in vec2 TexCoord;\n"
                                              "layout(location = 2) in vec4 Color;\n"
                                              "layout(location = 3) in float TexIndex;\n"

                                              "layout(location = 0) out vec2 texCoord;\n"
                                              "layout(location = 1) out vec4 outColor;\n"
                                              "layout(location = 2) out flat float texIndex;\n"

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
                                              "}";

const std::string s_BatchTextureFragmentShaderSource = "#version 450 core\n"

                                                       "layout (location = 0) out vec4 FragColor;\n"

                                                       "layout (location = 0) in vec2 texCoord;\n"
                                                       "layout (location = 1) in vec4 outColor;\n"
                                                       "layout (location = 2) in flat float texIndex;\n"

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
                                                       "}\n";

const std::string s_BatchFontFragmentShaderSource = "#version 450 core\n"

                                                    "layout (location = 0) out vec4 FragColor;\n"

                                                    "layout (location = 0) in vec2 texCoord;\n"
                                                    "layout (location = 1) in vec4 outColor;\n"
                                                    "layout (location = 2) in flat float texIndex;\n"

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
                                                    "}\n";

const std::string s_BatchSDFFragmentShaderSource = "#version 450 core\n"

                                                   "layout (location = 0) out vec4 FragColor;\n"

                                                   "layout (location = 0) in vec2 texCoord;\n"
                                                   "layout (location = 1) in vec4 outColor;\n"
                                                   "layout (location = 2) in flat float texIndex;\n"

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
                                                   "}\n";

const std::string s_RoundedRectFragmentShaderSource = "#version 450 core\n"
                                                      "layout (location = 0) out vec4 FragColor;\n"
                                                      "layout(location = 0) in vec2 texCoord;\n"
                                                      "layout(location = 1) in vec4 outColor;\n"
                                                      "layout(location = 2) in flat float texIndex;\n"
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
                                                      "float roundedBoxSDF(vec2 CenterPosition, vec2 Size, float Radius) {\n"
                                                      "return length(max(abs(CenterPosition)-Size+Radius,0.0))-Radius;\n"
                                                      "}\n"
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
                                                      "	vec2 size = vec2(1, 1);\n"
                                                      "	vec2 location = vec2(0, 0);\n"
                                                      "	float edgeSoftness = 1.0f;\n"
                                                      "	float radius = 50.0f;\n"
                                                      "	float distance = roundedBoxSDF(texCoord.xy - location - (size / 2.0f), size / 2.0f, radius);\n"
                                                      "	float smoothedAlpha = 1.0f - smoothstep(0.0, edgeSoftness * 2.0f, distance);\n"
                                                      "	float shadowSoftness = 30.0f;\n"
                                                      "	vec2 shadowOffset = vec2(0.0f, 10.0f);\n"
                                                      "	float shadowDistance = roundedBoxSDF(texCoord.xy - location + shadowOffset - (size / 2.0f), size / 2.0f, radius);\n"
                                                      "	float shadowAlpha = 1.0f - smoothstep(-shadowSoftness, shadowSoftness, shadowDistance);\n"
                                                      "	vec4 shadowColor = vec4(0.4f, 0.4f, 0.4f, 1.0f);\n"
                                                      "	FragColor = mix(FragColor, shadowColor, shadowAlpha - smoothedAlpha);\n"
                                                      "}";

namespace Nexus::Graphics
{
Nexus::Graphics::ResourceSetSpecification GetResourceSetSpecification()
{
    Nexus::Graphics::ResourceSetSpecification resourceSpec;
    resourceSpec.SampledImages = {
        {"texture0", 1, 0, Nexus::Graphics::ResourceType::CombinedImageSampler},   {"texture1", 1, 1, Nexus::Graphics::ResourceType::CombinedImageSampler},
        {"texture2", 1, 2, Nexus::Graphics::ResourceType::CombinedImageSampler},   {"texture3", 1, 3, Nexus::Graphics::ResourceType::CombinedImageSampler},
        {"texture4", 1, 4, Nexus::Graphics::ResourceType::CombinedImageSampler},   {"texture5", 1, 5, Nexus::Graphics::ResourceType::CombinedImageSampler},
        {"texture6", 1, 6, Nexus::Graphics::ResourceType::CombinedImageSampler},   {"texture7", 1, 7, Nexus::Graphics::ResourceType::CombinedImageSampler},
        {"texture8", 1, 8, Nexus::Graphics::ResourceType::CombinedImageSampler},   {"texture9", 1, 9, Nexus::Graphics::ResourceType::CombinedImageSampler},
        {"texture10", 1, 10, Nexus::Graphics::ResourceType::CombinedImageSampler}, {"texture11", 1, 11, Nexus::Graphics::ResourceType::CombinedImageSampler},
        {"texture12", 1, 12, Nexus::Graphics::ResourceType::CombinedImageSampler}, {"texture13", 1, 13, Nexus::Graphics::ResourceType::CombinedImageSampler},
        {"texture14", 1, 14, Nexus::Graphics::ResourceType::CombinedImageSampler}, {"texture15", 1, 15, Nexus::Graphics::ResourceType::CombinedImageSampler}};

    resourceSpec.UniformBuffers = {{"MVP", 0, 0, Nexus::Graphics::ResourceType::UniformBuffer}};

    return resourceSpec;
}

const uint32_t MAX_VERTEX_COUNT = 1024;
const uint32_t MAX_TEXTURE_COUNT = 16;

bool FindTextureInBatch(BatchInfo &info, Ref<Texture2D> texture, uint32_t &index)
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

float GetOrCreateTexIndex(BatchInfo &info, Nexus::Ref<Nexus::Graphics::Texture2D> texture)
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

void FlushTextures(BatchInfo &info, Nexus::Ref<Nexus::Graphics::Texture2D> blankTexture)
{
    info.Textures.clear();
    info.Textures.push_back(blankTexture);
}

void ResetBatcher(BatchInfo &info, Nexus::Ref<Nexus::Graphics::Texture2D> blankTexture)
{
    info.Vertices.clear();
    info.Indices.clear();
    info.VertexCount = 0;
    info.IndexCount = 0;
    info.ShapeCount = 0;
    FlushTextures(info, blankTexture);
}

void CreateBatcher(BatchInfo &info, Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderTarget target, Nexus::Ref<Nexus::Graphics::ShaderModule> vertexModule,
                   Nexus::Ref<Nexus::Graphics::ShaderModule> fragmentModule)
{
    info.Vertices.resize(MAX_VERTEX_COUNT);
    info.Indices.resize(MAX_VERTEX_COUNT * 3);
    info.Textures.resize(MAX_TEXTURE_COUNT);

    Nexus::Graphics::PipelineDescription description;
    description.RasterizerStateDesc.TriangleCullMode = Nexus::Graphics::CullMode::None;
    description.Layouts = {Nexus::Graphics::VertexPositionTexCoordColorTexIndex::GetLayout()};
    description.VertexModule = vertexModule;
    description.FragmentModule = fragmentModule;
    description.ResourceSetSpec = GetResourceSetSpecification();

    description.ColourFormats[0] = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
    description.ColourTargetCount = 1;
    description.DepthFormat = Nexus::Graphics::PixelFormat::D24_UNorm_S8_UInt;

    description.BlendStateDesc.EnableBlending = true;
    description.BlendStateDesc.SourceColourBlend = BlendFactor::SourceAlpha;
    description.BlendStateDesc.DestinationColourBlend = BlendFactor::OneMinusSourceAlpha;
    description.BlendStateDesc.ColorBlendFunction = BlendEquation::Add;
    description.BlendStateDesc.SourceAlphaBlend = BlendFactor::SourceAlpha;
    description.BlendStateDesc.DestinationAlphaBlend = BlendFactor::OneMinusSourceAlpha;
    description.BlendStateDesc.AlphaBlendFunction = BlendEquation::Add;

    description.ColourTargetSampleCount = device->GetPrimaryWindow()->GetSwapchain()->GetSpecification().Samples;

    info.Pipeline = device->CreatePipeline(description);
    info.ResourceSet = device->CreateResourceSet(info.Pipeline);

    Nexus::Graphics::BufferDescription vertexBufferDesc;
    vertexBufferDesc.Size = info.Vertices.size() * sizeof(VertexPositionTexCoordColorTexIndex);
    vertexBufferDesc.Usage = BufferUsage::Dynamic;
    info.VertexBuffer = device->CreateVertexBuffer(vertexBufferDesc, nullptr);

    Nexus::Graphics::BufferDescription indexBufferDesc;
    indexBufferDesc.Size = info.Indices.size() * sizeof(uint32_t);
    indexBufferDesc.Usage = BufferUsage::Dynamic;
    info.IndexBuffer = device->CreateIndexBuffer(indexBufferDesc, nullptr);
}

BatchRenderer::BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderTarget target)
    : m_Device(device), m_RenderTarget(target), m_CommandList(m_Device->CreateCommandList())
{
    uint32_t textureData = 0xFFFFFFFF;

    Nexus::Graphics::Texture2DSpecification textureSpec;
    textureSpec.Width = 1;
    textureSpec.Height = 1;
    textureSpec.Format = PixelFormat::R8_G8_B8_A8_UNorm;
    m_BlankTexture = m_Device->CreateTexture2D(textureSpec);
    m_BlankTexture->SetData(&textureData, 0, 0, 0, 1, 1);

    Nexus::Ref<Nexus::Graphics::ShaderModule> vertexModule =
        device->CreateShaderModuleFromSpirvSource(s_BatchVertexShaderSource, "Batch Renderer - Vertex Shader", Nexus::Graphics::ShaderStage::Vertex);
    Nexus::Ref<Nexus::Graphics::ShaderModule> sdfFragmentModule =
        device->CreateShaderModuleFromSpirvSource(s_BatchSDFFragmentShaderSource, "Batch Renderer - SDF Fragment Shader", Nexus::Graphics::ShaderStage::Fragment);
    Nexus::Ref<Nexus::Graphics::ShaderModule> textureFragmentModule =
        device->CreateShaderModuleFromSpirvSource(s_BatchTextureFragmentShaderSource, "Batch Renderer - Texture Fragment Shader", Nexus::Graphics::ShaderStage::Fragment);
    Nexus::Ref<Nexus::Graphics::ShaderModule> fontFragmentModule =
        device->CreateShaderModuleFromSpirvSource(s_BatchFontFragmentShaderSource, "Batch Renderer - Font Fragment Shader", Nexus::Graphics::ShaderStage::Fragment);

    CreateBatcher(m_SDFBatchInfo, device, target, vertexModule, sdfFragmentModule);
    CreateBatcher(m_TextureBatchInfo, device, target, vertexModule, textureFragmentModule);
    CreateBatcher(m_FontBatchInfo, device, target, vertexModule, fontFragmentModule);

    Nexus::Graphics::BufferDescription uniformBufferDesc;
    uniformBufferDesc.Size = sizeof(glm::mat4);
    uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
    m_UniformBuffer = m_Device->CreateUniformBuffer(uniformBufferDesc, nullptr);

    Nexus::Graphics::SamplerSpecification samplerSpec{};
    samplerSpec.SampleFilter = Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear;
    m_Sampler = m_Device->CreateSampler(samplerSpec);
}

void BatchRenderer::Resize()
{
    m_Width = m_Device->GetPrimaryWindow()->GetWindowSize().X;
    m_Height = m_Device->GetPrimaryWindow()->GetWindowSize().Y;
}

void BatchRenderer::Begin(Viewport viewport, Scissor scissor)
{
    if (m_IsStarted)
    {
        throw std::runtime_error("Batching has already started");
    }

    m_IsStarted = true;

    ResetBatcher(m_TextureBatchInfo, m_BlankTexture);
    ResetBatcher(m_SDFBatchInfo, m_BlankTexture);
    ResetBatcher(m_FontBatchInfo, m_BlankTexture);

    m_Viewport = viewport;
    m_ScissorRectangle = scissor;

    glm::mat4 projection = glm::ortho<float>(m_Viewport.X, m_Viewport.Width, m_Viewport.Height, m_Viewport.Y, -1.0f, 1.0f);
    m_UniformBuffer->SetData(&projection, sizeof(projection));
}

void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color)
{
    DrawQuadFill(min, max, color, m_BlankTexture);
}

void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture2D> texture)
{
    DrawQuadFill(min, max, color, texture, 1.0f);
}

void BatchRenderer::DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture2D> texture, float tilingFactor)
{
    const float texIndex = GetOrCreateTexIndex(m_TextureBatchInfo, texture);

    EnsureStarted();

    const uint32_t shapeVertexCount = 4;
    const uint32_t shapeIndexCount = 6;

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

    VertexPositionTexCoordColorTexIndex v0;
    v0.Position = a;
    v0.TexCoords = {0.0f, 0.0f};
    v0.Color = color;
    v0.TexIndex = texIndex;
    m_TextureBatchInfo.Vertices.push_back(v0);

    VertexPositionTexCoordColorTexIndex v1;
    v1.Position = b;
    v1.TexCoords = {tilingFactor, 0.0f};
    v1.Color = color;
    v1.TexIndex = texIndex;
    m_TextureBatchInfo.Vertices.push_back(v1);

    VertexPositionTexCoordColorTexIndex v2;
    v2.Position = c;
    v2.TexCoords = {tilingFactor, tilingFactor};
    v2.Color = color;
    v2.TexIndex = texIndex;
    m_TextureBatchInfo.Vertices.push_back(v2);

    VertexPositionTexCoordColorTexIndex v3;
    v3.Position = d;
    v3.TexCoords = {0.0f, tilingFactor};
    v3.Color = color;
    v3.TexIndex = texIndex;
    m_TextureBatchInfo.Vertices.push_back(v3);

    m_TextureBatchInfo.IndexCount += shapeIndexCount;
    m_TextureBatchInfo.VertexCount += shapeVertexCount;
}

void BatchRenderer::DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color)
{
    DrawQuadFill(rectangle, color, m_BlankTexture);
}

void BatchRenderer::DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture2D> texture)
{
    DrawQuadFill(rectangle, color, texture, 1.0f);
}

void BatchRenderer::DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture2D> texture, float tilingFactor)
{
    glm::vec2 min = {(float)rectangle.GetLeft(), (float)rectangle.GetTop()};
    glm::vec2 max = {(float)rectangle.GetRight(), (float)rectangle.GetBottom()};
    DrawQuadFill(min, max, color, texture, tilingFactor);
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
    const uint32_t shapeIndexCount = 6;

    Nexus::Graphics::BatchInfo *info = &m_FontBatchInfo;

    // if we are using a SDF font, then we need to use an SDF pipeline
    // otherwise just use a regular rasterized font pipeline
    if (font->GetFontType() == Nexus::Graphics::FontType::SDF)
    {
        info = &m_SDFBatchInfo;
    }

    EnsureSpace(*info, shapeVertexCount, shapeIndexCount);

    const auto &characterInfo = font->GetCharacter(character);
    glm::vec2 min = position;
    glm::vec2 max = {position.x + size.x, position.y + size.y};

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

    VertexPositionTexCoordColorTexIndex v0;
    v0.Position = a;
    v0.TexCoords = {characterInfo.TexCoordsMin.x, characterInfo.TexCoordsMin.y};
    v0.Color = color;
    v0.TexIndex = texIndex;
    info->Vertices.push_back(v0);

    VertexPositionTexCoordColorTexIndex v1;
    v1.Position = b;
    v1.TexCoords = {characterInfo.TexCoordsMax.x, characterInfo.TexCoordsMin.y};
    v1.Color = color;
    v1.TexIndex = texIndex;
    info->Vertices.push_back(v1);

    VertexPositionTexCoordColorTexIndex v2;
    v2.Position = c;
    v2.TexCoords = {characterInfo.TexCoordsMax.x, characterInfo.TexCoordsMax.y};
    v2.Color = color;
    v2.TexIndex = texIndex;
    info->Vertices.push_back(v2);

    VertexPositionTexCoordColorTexIndex v3;
    v3.Position = d;
    v3.TexCoords = {characterInfo.TexCoordsMin.x, characterInfo.TexCoordsMax.y};
    v3.Color = color;
    v3.TexIndex = texIndex;
    info->Vertices.push_back(v3);

    info->IndexCount += shapeIndexCount;
    info->VertexCount += shapeVertexCount;
}

void BatchRenderer::DrawString(const std::string &text, const glm::vec2 &position, uint32_t size, const glm::vec4 &color, Font *font)
{
    // because all fonts we use are SDFs, we need to calculate how big the generated quads should be from the original size of the glyph
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
    const uint32_t shapeIndexCount = 6;

    EnsureSpace(m_TextureBatchInfo, shapeVertexCount, shapeIndexCount);

    thickness = glm::clamp(thickness, 1.0f, 10.0f);

    glm::vec2 e1 = b - a;
    e1 = glm::normalize(e1);
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

    VertexPositionTexCoordColorTexIndex v0;
    v0.Position = glm::vec3(q1, 0.0f);
    v0.TexCoords = {0.0f, 0.0f};
    v0.Color = color;
    v0.TexIndex = 0;
    m_TextureBatchInfo.Vertices.push_back(v0);

    VertexPositionTexCoordColorTexIndex v1;
    v1.Position = glm::vec3(q2, 0.0f);
    v1.TexCoords = {1.0f, 0.0f};
    v1.Color = color;
    v1.TexIndex = 0;
    m_TextureBatchInfo.Vertices.push_back(v1);

    VertexPositionTexCoordColorTexIndex v2;
    v2.Position = glm::vec3(q3, 0.0f);
    v2.TexCoords = {1.0f, 1.0f};
    v2.Color = color;
    v2.TexIndex = 0;
    m_TextureBatchInfo.Vertices.push_back(v2);

    VertexPositionTexCoordColorTexIndex v3;
    v3.Position = glm::vec3(q4, 0.0f);
    v3.TexCoords = {0.0f, 1.0f};
    v3.Color = color;
    v3.TexIndex = 0;
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
    float angle = glm::radians(180.0f);

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

void BatchRenderer::DrawCircleRegionFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float startAngle, float fillAngle)
{
    DrawCircleRegionFill(position, radius, color, numberOfPoints, startAngle, fillAngle, m_BlankTexture);
}

void BatchRenderer::DrawCircleRegionFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float startAngle, float fillAngle,
                                         Ref<Texture2D> texture)
{
    DrawCircleRegionFill(position, radius, color, numberOfPoints, startAngle, fillAngle, texture, 1.0f);
}

void BatchRenderer::DrawCircleRegionFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float startAngle, float fillAngle,
                                         Ref<Texture2D> texture, float tilingFactor)
{
    const uint32_t minPoints = 3;
    const uint32_t maxPoints = 256;

    numberOfPoints = glm::clamp<float>(numberOfPoints, minPoints, maxPoints);
    float deltaAngle = glm::radians(fillAngle) / (float)numberOfPoints;
    float currentAngle = glm::radians(startAngle) + glm::radians(180.0f); // start rendering the circle from the centre top

    const glm::vec2 topLeft = {position.x - radius, position.y - radius};
    const glm::vec2 bottomRight = {position.x + radius, position.y + radius};

    const glm::vec3 centre = {position.x, position.y, 0.0f};
    const glm::vec2 uvTL = {0.0f, 0.0f};
    const glm::vec2 uvBR = {tilingFactor, tilingFactor};

    glm::vec2 centreUV = {Nexus::Utils::ReMapRange(topLeft.x, bottomRight.x, uvTL.x, uvBR.x, centre.x),
                          Nexus::Utils::ReMapRange(topLeft.y, bottomRight.y, uvTL.y, uvBR.y, centre.y)};

    for (int i = 0; i < numberOfPoints; i++)
    {
        glm::vec3 posA = {glm::sin(currentAngle) * radius + (position.x), glm::cos(currentAngle) * radius + (position.y), 0.0f};

        glm::vec2 uvA = {Nexus::Utils::ReMapRange(topLeft.x, bottomRight.x, uvTL.x, uvBR.x, posA.x), Nexus::Utils::ReMapRange(topLeft.y, bottomRight.y, uvTL.y, uvBR.y, posA.y)};

        currentAngle -= deltaAngle;

        glm::vec3 posB = {glm::sin(currentAngle) * radius + (position.x), glm::cos(currentAngle) * radius + (position.y), 0.0f};

        glm::vec2 uvB = {Nexus::Utils::ReMapRange(topLeft.x, bottomRight.x, uvTL.x, uvBR.x, posB.x), Nexus::Utils::ReMapRange(topLeft.y, bottomRight.y, uvTL.y, uvBR.y, posB.y)};

        DrawTriangle(centre, centreUV, posA, uvA, posB, uvB, color, texture);
    }
}

void BatchRenderer::DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture2D> texture)
{
    DrawCircleRegionFill(position, radius, color, numberOfPoints, 0.0f, 360.0f, texture);
}

void BatchRenderer::DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints)
{
    DrawCircleFill(circle, color, numberOfPoints, m_BlankTexture);
}

void BatchRenderer::DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture2D> texture)
{
    const auto &pos = circle.GetPosition();
    DrawCircleFill({pos.X, pos.Y}, circle.GetRadius(), color, numberOfPoints, texture);
}

void BatchRenderer::DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture2D> texture, float tilingFactor)
{
    DrawCircleRegionFill({circle.GetPosition().X, circle.GetPosition().Y}, circle.GetRadius(), color, numberOfPoints, 0.0f, 360.0f, texture, tilingFactor);
}

void BatchRenderer::DrawCross(const Rectangle<float> &rectangle, float thickness, const glm::vec4 &color)
{
    glm::vec2 topLeft = {rectangle.GetLeft() + thickness, rectangle.GetTop() + thickness};
    glm::vec2 bottomRight = {rectangle.GetRight() - thickness, rectangle.GetBottom() - thickness};
    glm::vec2 bottomLeft = {rectangle.GetLeft() + thickness, rectangle.GetBottom() - thickness};
    glm::vec2 topRight = {rectangle.GetRight() - thickness, rectangle.GetTop() + thickness};

    DrawLine(topLeft, bottomRight, color, thickness);
    DrawLine(bottomLeft, topRight, color, thickness);
}

void BatchRenderer::DrawTriangle(const glm::vec3 &pos0, const glm::vec2 &uv0, const glm::vec3 &pos1, const glm::vec2 &uv1, const glm::vec3 &pos2, const glm::vec2 &uv2,
                                 const glm::vec4 &color)
{
    DrawTriangle(pos0, uv0, pos1, uv1, pos2, uv2, color, m_BlankTexture);
}

void BatchRenderer::DrawTriangle(const glm::vec3 &pos0, const glm::vec2 &uv0, const glm::vec3 &pos1, const glm::vec2 &uv1, const glm::vec3 &pos2, const glm::vec2 &uv2,
                                 const glm::vec4 &color, Ref<Texture2D> texture)
{
    float texIndex = GetOrCreateTexIndex(m_TextureBatchInfo, texture);

    EnsureStarted();

    const uint32_t shapeVertexCount = 3;
    const uint32_t shapeIndexCount = 3;

    EnsureSpace(m_TextureBatchInfo, shapeVertexCount, shapeIndexCount);

    m_TextureBatchInfo.Indices.push_back(0 + m_TextureBatchInfo.VertexCount);
    m_TextureBatchInfo.Indices.push_back(1 + m_TextureBatchInfo.VertexCount);
    m_TextureBatchInfo.Indices.push_back(2 + m_TextureBatchInfo.VertexCount);

    VertexPositionTexCoordColorTexIndex v0;
    v0.Position = pos0;
    v0.TexCoords = uv0;
    v0.Color = color;
    v0.TexIndex = texIndex;
    m_TextureBatchInfo.Vertices.push_back(v0);

    VertexPositionTexCoordColorTexIndex v1;
    v1.Position = pos1;
    v1.TexCoords = uv1;
    v1.Color = color;
    v1.TexIndex = texIndex;
    m_TextureBatchInfo.Vertices.push_back(v1);

    VertexPositionTexCoordColorTexIndex v2;
    v2.Position = pos2;
    v2.TexCoords = uv2;
    v2.Color = color;
    v2.TexIndex = texIndex;
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

void BatchRenderer::DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture2D> texture)
{
    DrawPolygonFill(polygon, color, m_BlankTexture, 1.0f);
}

void BatchRenderer::DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture2D> texture, float tilingFactor)
{
    const auto &boundingRectangle = polygon.GetBoundingRectangle();
    const std::vector<Triangle2D> &tris = polygon.GetTriangles();

    const glm::vec2 uvTL = {0.0f, 0.0f};
    const glm::vec2 uvBR = {tilingFactor, tilingFactor};

    for (const auto &triangle2D : tris)
    {
        const Triangle3D triToDraw = triangle2D;

        const glm::vec2 uvA = {Nexus::Utils::ReMapRange(boundingRectangle.GetLeft(), boundingRectangle.GetRight(), uvTL.x, uvBR.x, triToDraw.A.x),
                               Nexus::Utils::ReMapRange(boundingRectangle.GetTop(), boundingRectangle.GetBottom(), uvTL.y, uvBR.y, triToDraw.A.y)};

        const glm::vec2 uvB = {Nexus::Utils::ReMapRange(boundingRectangle.GetLeft(), boundingRectangle.GetRight(), uvTL.x, uvBR.x, triToDraw.B.x),
                               Nexus::Utils::ReMapRange(boundingRectangle.GetTop(), boundingRectangle.GetBottom(), uvTL.y, uvBR.y, triToDraw.B.y)};

        const glm::vec2 uvC = {Nexus::Utils::ReMapRange(boundingRectangle.GetLeft(), boundingRectangle.GetRight(), uvTL.x, uvBR.x, triToDraw.C.x),
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

void BatchRenderer::DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, Ref<Texture2D> texture)
{
    DrawRoundedRectangleFill(roundedRectangle, color, texture, 1.0f);
}

void BatchRenderer::DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, Ref<Texture2D> texture, float tilingFactor)
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
    uint32_t maxIndexCount = info.Indices.capacity();

    if (shapeVertexCount > maxVertexCount || shapeIndexCount > maxIndexCount)
    {
        throw std::runtime_error("Max vertex or index count reached for one draw call");
    }

    if (info.VertexCount + shapeVertexCount > maxVertexCount || info.IndexCount + shapeIndexCount > maxIndexCount || info.Textures.size() > MAX_TEXTURE_COUNT)
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

    info.VertexBuffer->SetData(info.Vertices.data(), info.Vertices.size() * sizeof(VertexPositionTexCoordColorTexIndex));
    info.IndexBuffer->SetData(info.Indices.data(), info.Indices.size() * sizeof(uint32_t));

    info.ResourceSet->WriteUniformBuffer(m_UniformBuffer, "MVP");

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
    m_CommandList->SetPipeline(info.Pipeline);
    m_CommandList->SetRenderTarget(Nexus::Graphics::RenderTarget(m_Device->GetPrimaryWindow()->GetSwapchain()));
    m_CommandList->SetViewport(m_Viewport);
    m_CommandList->SetScissor(m_ScissorRectangle);
    m_CommandList->SetResourceSet(info.ResourceSet);
    m_CommandList->SetVertexBuffer(info.VertexBuffer, 0);
    m_CommandList->SetIndexBuffer(info.IndexBuffer);
    m_CommandList->DrawIndexed(info.IndexCount, 0, 0);
    m_CommandList->End();
    m_Device->SubmitCommandList(m_CommandList);

    FlushTextures(info, m_BlankTexture);
}
} // namespace Nexus::Graphics