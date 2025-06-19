#pragma once

#include "Nexus-Core/Graphics/Circle.hpp"
#include "Nexus-Core/Graphics/Font.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Polygon.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Graphics/RoundedRectangle.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/Vertex.hpp"

namespace Nexus::Graphics
{
	struct BatchVertex
	{
		glm::vec4		  Color		= {1.0f, 1.0f, 1.0f, 1.0f};
		glm::vec3		  Position	= {0, 0, 0};
		float			  TexIndex	= 0.0f;
		glm::vec2		  TexCoords = {0, 0};
		Point2D<uint32_t> Id		= {0, 0};

		BatchVertex() = default;

		BatchVertex(const glm::vec3 &position, const glm::vec2 &texCoords, const glm::vec4 &color, float texIndex, Point2D<uint32_t> id)
			: Position(position),
			  TexCoords(texCoords),
			  Color(color),
			  TexIndex(texIndex),
			  Id(id)
		{
		}

		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout = Graphics::VertexBufferLayout({{Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"},
																					   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
																					   {Nexus::Graphics::ShaderDataType::Float, "TEXCOORD"},
																					   {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
																					   {Nexus::Graphics::ShaderDataType::UInt2, "TEXCOORD"}},
																					  Graphics::StepRate::Vertex);
			return layout;
		}
	};

	struct BatchInfo
	{
		Nexus::Ref<Nexus::Graphics::GraphicsPipeline> Pipeline	  = nullptr;
		Nexus::Ref<Nexus::Graphics::ResourceSet> ResourceSet = nullptr;

		std::vector<Nexus::Graphics::BatchVertex>			Vertices;
		std::vector<uint32_t>								Indices;
		std::vector<Nexus::Ref<Nexus::Graphics::Texture>>	Textures;

		uint32_t ShapeCount	 = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount	 = 0;

		Nexus::Ref<Nexus::Graphics::DeviceBuffer> VertexUploadBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> VertexBuffer		 = nullptr;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> IndexUploadBuffer	 = nullptr;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer> IndexBuffer		 = nullptr;
	};

	class NX_API BatchRenderer
	{
	  public:
		BatchRenderer(Nexus::Graphics::GraphicsDevice *device, bool useDepthTest, uint32_t sampleCount);

		void Resize();

		void Begin(Nexus::Graphics::RenderTarget target, Viewport viewport, Scissor scissor);
		void Begin(Nexus::Graphics::RenderTarget target, Viewport viewport, Scissor scissor, const glm::mat4 &camera);

		void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color);
		void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture> texture);
		void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture> texture, float tilingFactor);
		void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color);
		void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture> texture);
		void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture> texture, float tilingFactor);
		void DrawQuadFill(const glm::vec4 &color, Ref<Texture> texture, float tilingFactor, const glm::mat4 &transform, Nexus::GUID id);
		void DrawQuad(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, float thickness);
		void DrawQuad(const Rectangle<float> &rectangle, const glm::vec4 &color, float thickness);
		void DrawCharacter(char character, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, Font *font);
		void DrawString(const std::string &text, const glm::vec2 &position, uint32_t size, const glm::vec4 &color, Font *font);
		void DrawLine(const glm::vec2 &a, const glm::vec2 &b, const glm::vec4 &color, float thickness);
		void DrawCircle(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float thickness);
		void DrawCircle(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, float thickness);
		void DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints);

		void DrawCircleRegionFill(const glm::vec2 &position,
								  float			   radius,
								  const glm::vec4 &color,
								  uint32_t		   numberOfPoints,
								  float			   startAngle,
								  float			   fillAngle);

		void DrawCircleRegionFill(const glm::vec2 &position,
								  float			   radius,
								  const glm::vec4 &color,
								  uint32_t		   numberOfPoints,
								  float			   startAngle,
								  float			   fillAngle,
								  Ref<Texture>	   texture);

		void DrawCircleRegionFill(const glm::vec2 &position,
								  float			   radius,
								  const glm::vec4 &color,
								  uint32_t		   numberOfPoints,
								  float			   startAngle,
								  float			   fillAngle,
								  Ref<Texture>	   texture,
								  float			   tilingFactor);

		void DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture> texture);
		void DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints);
		void DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture> texture);

		void DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture> texture, float tilingFactor);

		void DrawCross(const Rectangle<float> &rectangle, float thickness, const glm::vec4 &color);
		void DrawTriangle(const glm::vec3 &pos0,
						  const glm::vec2 &uv0,
						  const glm::vec3 &pos1,
						  const glm::vec2 &uv1,
						  const glm::vec3 &pos2,
						  const glm::vec2 &uv2,
						  const glm::vec4 &color);

		void DrawTriangle(const glm::vec3 &pos0,
						  const glm::vec2 &uv0,
						  const glm::vec3 &pos1,
						  const glm::vec2 &uv1,
						  const glm::vec3 &pos2,
						  const glm::vec2 &uv2,
						  const glm::vec4 &color,
						  Ref<Texture>	   texture);

		void DrawTriangle(const Graphics::Triangle2D &tri, const glm::vec4 &color);
		void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color);
		void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture> texture);
		void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture> texture, float tilingFactor);
		void DrawRoundedRectangle(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, float thickness);
		void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color);
		void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, Ref<Texture> texture);
		void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, Ref<Texture> texture, float tilingFactor);
		void End();

	  private:
		void Flush();
		void EnsureStarted();
		void EnsureSpace(BatchInfo &info, uint32_t shapeVertexCount, uint32_t shapeIndexCount);
		void PerformDraw(BatchInfo &info);

	  private:
		Nexus::Graphics::GraphicsDevice			*m_Device	   = nullptr;
		Nexus::Ref<Nexus::Graphics::CommandList> m_CommandList = nullptr;
		Nexus::Ref<Nexus::Graphics::Sampler>	 m_Sampler	   = nullptr;
		bool									 m_IsStarted   = false;

		Nexus::Ref<Nexus::Graphics::Texture>	   m_BlankTexture		 = nullptr;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_UniformUploadBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::DeviceBuffer>  m_UniformBuffer		 = nullptr;

		uint32_t					  m_Width  = 0;
		uint32_t					  m_Height = 0;
		Nexus::Graphics::RenderTarget m_RenderTarget;

		Nexus::Graphics::Viewport m_Viewport;
		Nexus::Graphics::Scissor  m_ScissorRectangle;

		BatchInfo m_TextureBatchInfo;
		BatchInfo m_SDFBatchInfo;
		BatchInfo m_FontBatchInfo;

		bool m_UseDepthTest = false;
	};
}	 // namespace Nexus::Graphics