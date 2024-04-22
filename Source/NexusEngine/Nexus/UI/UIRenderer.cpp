#include "UIRenderer.hpp"

#include <glm/glm.hpp>
#include "glm/gtx/compatibility.hpp"

const std::string s_UIVertexShaderSource =
    "#version 450 core\n"

    "layout(location = 0) in vec3 Position;\n"
    "layout(location = 1) in vec2 TexCoord;\n"
    "layout(location = 2) in vec4 Color;\n"

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
    "}";

const std::string s_UI_Texture_FragmentShaderSource =
    "#version 450 core\n"

    "layout (location = 0) out vec4 FragColor;\n"

    "layout (location = 0) in vec2 texCoord;\n"
    "layout (location = 1) in vec4 outColor;\n"

    "layout (set = 1, binding = 0) uniform sampler2D texSampler;\n"

    "void main()\n"
    "{\n"
    "   FragColor = texture(texSampler, texCoord);\n"
    "}\n";

const std::string s_UI_SDF_FragmentShaderSource =
    "#version 450 core\n"

    "layout (location = 0) out vec4 FragColor;\n"

    "layout (location = 0) in vec2 texCoord;\n"
    "layout (location = 1) in vec4 outColor;\n"

    "layout (set = 1, binding = 0) uniform sampler2D texSampler;\n"

    "void main()\n"
    "{\n"
    "   float alpha = texture(texSampler, texCoord).r;\n"
    "   FragColor = vec4(outColor.rgb, alpha);\n"
    "}\n";

namespace Nexus::UI
{
    UIRenderer::UIRenderer(Nexus::Graphics::GraphicsDevice *device, Graphics::RenderTarget target)
        : m_Device(device), m_RenderTarget(target)
    {
        m_CommandList = m_Device->CreateCommandList();
        CreatePipelines();

        Nexus::Graphics::BufferDescription uniformBufferDesc;
        uniformBufferDesc.Size = sizeof(glm::mat4);
        uniformBufferDesc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
        m_UniformBuffer = m_Device->CreateUniformBuffer(uniformBufferDesc, nullptr);

        uint32_t textureData = 0xFFFFFFFF;

        Nexus::Graphics::TextureSpecification textureSpec;
        textureSpec.Width = 1;
        textureSpec.Height = 1;
        textureSpec.Format = Nexus::Graphics::PixelFormat::R8_G8_B8_A8_UNorm;
        m_BlankTexture = m_Device->CreateTexture(textureSpec);
        // m_BlankTexture->SetData(&textureData, 0, 0, 0, 1, 1);
        m_BlankTexture->SetData(&textureData, sizeof(textureData), 0);

        Nexus::Graphics::SamplerSpecification samplerSpec{};
        samplerSpec.SampleFilter = Nexus::Graphics::SamplerFilter::MinLinear_MagLinear_MipLinear;
        m_Sampler = m_Device->CreateSampler(samplerSpec);
    }

    void UIRenderer::Begin() const
    {
        m_CommandList->Begin();
    }

    void UIRenderer::SetViewport(const Nexus::Graphics::Viewport &vp) const
    {
        m_CommandList->SetViewport(vp);

        glm::mat4 projection = glm::ortho<float>(vp.X, vp.Width, vp.Height, vp.Y, -1.0f, 1.0f);
        m_UniformBuffer->SetData(&projection, sizeof(projection));
    }

    void UIRenderer::SetScissor(const Nexus::Graphics::Scissor &scissor) const
    {
        m_CommandList->SetScissor(scissor);
    }

    void UIRenderer::Clear(const Nexus::Graphics::ClearColorValue &clearColor) const
    {
        m_CommandList->ClearColorTarget(0, clearColor);
    }

    void UIRenderer::DrawRectangle(const Nexus::Graphics::Rectangle<float> &rect, const glm::vec4 &color)
    {
        Nexus::Graphics::Rectangle<float> uvs(0, 0, 1, 1);
        glm::vec2 uv1 = {0, 0};
        glm::vec2 uv2 = {1, 1};
        DrawRectangle(rect, uv1, uv2, color, m_BlankTexture);
    }

    void UIRenderer::DrawRectangle(const Nexus::Graphics::Rectangle<float> &rect, const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec4 &color, Nexus::Ref<Nexus::Graphics::Texture> texture)
    {
        m_Vertices.clear();
        m_Indices.clear();

        glm::vec2 minPos = {(float)rect.GetLeft(), (float)rect.GetTop()};
        glm::vec2 maxPos = {(float)rect.GetRight(), (float)rect.GetBottom()};

        const uint32_t shapeVertexCount = 4;
        const uint32_t shapeIndexCount = 6;

        glm::vec3 a(minPos.x, maxPos.y, 0.0f);
        glm::vec3 b(maxPos.x, maxPos.y, 0.0f);
        glm::vec3 c(maxPos.x, minPos.y, 0.0f);
        glm::vec3 d(minPos.x, minPos.y, 0.0f);

        m_Indices.push_back(0);
        m_Indices.push_back(1);
        m_Indices.push_back(2);
        m_Indices.push_back(0);
        m_Indices.push_back(2);
        m_Indices.push_back(3);

        Graphics::VertexPositionTexCoordColor v0;
        v0.Position = a;
        v0.TexCoords = {uv1.x, uv2.y};
        v0.Color = color;
        m_Vertices.push_back(v0);

        Graphics::VertexPositionTexCoordColor v1;
        v1.Position = b;
        v1.TexCoords = {uv2.x, uv2.y};
        v1.Color = color;
        m_Vertices.push_back(v1);

        Graphics::VertexPositionTexCoordColor v2;
        v2.Position = c;
        v2.TexCoords = {uv2.x, uv1.y};
        v2.Color = color;
        m_Vertices.push_back(v2);

        Graphics::VertexPositionTexCoordColor v3;
        v3.Position = d;
        v3.TexCoords = {uv1.x, uv1.y};
        v3.Color = color;
        m_Vertices.push_back(v3);

        UpdateBuffers();

        m_CommandList->SetPipeline(m_TexturePipeline);

        m_TextureResourceSet->WriteCombinedImageSampler(texture, m_Sampler, "texSampler");
        m_TextureResourceSet->WriteUniformBuffer(m_UniformBuffer, "MVP");
        m_CommandList->SetResourceSet(m_TextureResourceSet);

        m_CommandList->SetVertexBuffer(m_VertexBuffer, 0);
        m_CommandList->SetIndexBuffer(m_IndexBuffer);
        m_CommandList->DrawIndexed(m_Indices.size(), 0, 0);
    }

    void UIRenderer::DrawCharacter(char character, const Nexus::Graphics::Rectangle<float> &rect, const glm::vec4 &color, Nexus::Graphics::Font *font)
    {
        const auto &characterInfo = font->GetCharacter(character);

        m_Vertices.clear();
        m_Indices.clear();

        glm::vec2 uv1 = {characterInfo.TexCoordsMin.x, characterInfo.TexCoordsMin.y};
        glm::vec2 uv2 = {characterInfo.TexCoordsMax.x, characterInfo.TexCoordsMax.y};

        const uint32_t shapeVertexCount = 4;
        const uint32_t shapeIndexCount = 6;

        glm::vec3 a(uv1.x, uv2.y, 0.0f);
        glm::vec3 b(uv2.x, uv2.y, 0.0f);
        glm::vec3 c(uv2.x, uv1.y, 0.0f);
        glm::vec3 d(uv1.x, uv1.y, 0.0f);

        m_Indices.push_back(0);
        m_Indices.push_back(1);
        m_Indices.push_back(2);
        m_Indices.push_back(0);
        m_Indices.push_back(2);
        m_Indices.push_back(3);

        Graphics::VertexPositionTexCoordColor v0;
        v0.Position = a;
        v0.TexCoords = {uv1.x, uv2.y};
        v0.Color = color;
        m_Vertices.push_back(v0);

        Graphics::VertexPositionTexCoordColor v1;
        v1.Position = b;
        v1.TexCoords = {uv2.x, uv2.y};
        v1.Color = color;
        m_Vertices.push_back(v1);

        Graphics::VertexPositionTexCoordColor v2;
        v2.Position = c;
        v2.TexCoords = {uv2.x, uv1.y};
        v2.Color = color;
        m_Vertices.push_back(v2);

        Graphics::VertexPositionTexCoordColor v3;
        v3.Position = d;
        v3.TexCoords = {uv1.x, uv1.y};
        v3.Color = color;
        m_Vertices.push_back(v3);

        UpdateBuffers();

        m_CommandList->SetPipeline(m_SDFPipeline);

        m_SDFResourceSet->WriteCombinedImageSampler(font->GetTexture(), m_Sampler, "texSampler");
        m_SDFResourceSet->WriteUniformBuffer(m_UniformBuffer, "MVP");
        m_CommandList->SetResourceSet(m_SDFResourceSet);

        m_CommandList->SetVertexBuffer(m_VertexBuffer, 0);
        m_CommandList->SetIndexBuffer(m_IndexBuffer);
        m_CommandList->DrawIndexed(m_Indices.size(), 0, 0);
    }

    void UIRenderer::End() const
    {
        m_CommandList->End();
    }

    Nexus::Ref<Nexus::Graphics::CommandList> UIRenderer::GetCommandList() const
    {
        return m_CommandList;
    }

    void UIRenderer::CreatePipelines()
    {
        // texture pipeline
        {
            Nexus::Graphics::PipelineDescription description;
            description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            description.Layouts = {Nexus::Graphics::VertexPositionTexCoordColor::GetLayout()};

            description.VertexModule = m_Device->CreateShaderModuleFromSpirvSource(s_UIVertexShaderSource, "Batch Renderer - Vertex Shader", Nexus::Graphics::ShaderStage::Vertex);
            description.FragmentModule = m_Device->CreateShaderModuleFromSpirvSource(s_UI_Texture_FragmentShaderSource, "Batch Renderer - Texture Fragment Shader", Nexus::Graphics::ShaderStage::Fragment);

            Nexus::Graphics::ResourceSetSpecification resourceSpec;
            resourceSpec.SampledImages =
                {
                    {"texSampler", 1, 0, Nexus::Graphics::ResourceType::CombinedImageSampler},
                };

            resourceSpec.UniformBuffers =
                {
                    {"MVP", 0, 0, Nexus::Graphics::ResourceType::UniformBuffer}};

            description.ResourceSetSpecification = resourceSpec;
            description.Target = {m_RenderTarget};

            description.BlendStateDescription.EnableBlending = true;
            description.BlendStateDescription.SourceColourBlend = Nexus::Graphics::BlendFactor::SourceAlpha;
            description.BlendStateDescription.DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
            description.BlendStateDescription.ColorBlendFunction = Nexus::Graphics::BlendEquation::Add;
            description.BlendStateDescription.SourceAlphaBlend = Nexus::Graphics::BlendFactor::SourceAlpha;
            description.BlendStateDescription.DestinationAlphaBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
            description.BlendStateDescription.AlphaBlendFunction = Nexus::Graphics::BlendEquation::Add;

            m_TexturePipeline = m_Device->CreatePipeline(description);
            m_TextureResourceSet = m_Device->CreateResourceSet(m_TexturePipeline);
        }

        // sdf pipeline
        {
            Nexus::Graphics::PipelineDescription description;
            description.RasterizerStateDescription.CullMode = Nexus::Graphics::CullMode::None;
            description.Layouts = {Nexus::Graphics::VertexPositionTexCoordColor::GetLayout()};

            description.VertexModule = m_Device->CreateShaderModuleFromSpirvSource(s_UIVertexShaderSource, "Batch Renderer - Vertex Shader", Nexus::Graphics::ShaderStage::Vertex);
            description.FragmentModule = m_Device->CreateShaderModuleFromSpirvSource(s_UI_SDF_FragmentShaderSource, "Batch Renderer - SDF Fragment Shader", Nexus::Graphics::ShaderStage::Fragment);

            Nexus::Graphics::ResourceSetSpecification resourceSpec;
            resourceSpec.SampledImages =
                {
                    {"texSampler", 1, 0, Nexus::Graphics::ResourceType::CombinedImageSampler},
                };

            resourceSpec.UniformBuffers =
                {
                    {"MVP", 0, 0, Nexus::Graphics::ResourceType::UniformBuffer}};

            description.ResourceSetSpecification = resourceSpec;
            description.Target = {m_RenderTarget};

            description.BlendStateDescription.EnableBlending = true;
            description.BlendStateDescription.SourceColourBlend = Nexus::Graphics::BlendFactor::SourceAlpha;
            description.BlendStateDescription.DestinationColourBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
            description.BlendStateDescription.ColorBlendFunction = Nexus::Graphics::BlendEquation::Add;
            description.BlendStateDescription.SourceAlphaBlend = Nexus::Graphics::BlendFactor::SourceAlpha;
            description.BlendStateDescription.DestinationAlphaBlend = Nexus::Graphics::BlendFactor::OneMinusSourceAlpha;
            description.BlendStateDescription.AlphaBlendFunction = Nexus::Graphics::BlendEquation::Add;

            m_SDFPipeline = m_Device->CreatePipeline(description);
            m_SDFResourceSet = m_Device->CreateResourceSet(m_TexturePipeline);
        }
    }

    void UIRenderer::UpdateBuffers()
    {
        uint32_t maxVertexSize = m_Vertices.size() * sizeof(m_Vertices[0]);
        uint32_t maxIndexSize = m_Indices.size() * sizeof(m_Indices[0]);

        if (!m_VertexBuffer || maxVertexSize > m_VertexBuffer->GetDescription().Size)
        {
            Nexus::Graphics::BufferDescription desc;
            desc.Size = maxVertexSize;
            desc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_VertexBuffer = m_Device->CreateVertexBuffer(desc, nullptr);
        }

        if (!m_IndexBuffer || maxIndexSize > m_IndexBuffer->GetDescription().Size)
        {
            Nexus::Graphics::BufferDescription desc;
            desc.Size = maxIndexSize;
            desc.Usage = Nexus::Graphics::BufferUsage::Dynamic;
            m_IndexBuffer = m_Device->CreateIndexBuffer(desc, nullptr);
        }

        m_VertexBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(m_Vertices[0]));
        m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size() * sizeof(m_Indices[0]));
    }
}
