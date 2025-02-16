#pragma once

#include "Nexus-Core/Graphics/Circle.hpp"
#include "Nexus-Core/Graphics/Font.hpp"
#include "Nexus-Core/Graphics/GraphicsDevice.hpp"
#include "Nexus-Core/Graphics/Polygon.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Graphics/RoundedRectangle.hpp"
#include "Nexus-Core/Vertex.hpp"

namespace Nexus::Graphics
{
	struct VertexPositionTexCoordColorTexIndex
	{
		glm::vec3 Position	= {0, 0, 0};
		glm::vec2 TexCoords = {0, 0};
		glm::vec4 Color		= {1.0f, 1.0f, 1.0f, 1.0f};
		float	  TexIndex	= 0.0f;

		NX_API VertexPositionTexCoordColorTexIndex() = default;

		NX_API VertexPositionTexCoordColorTexIndex(const glm::vec3 &position, const glm::vec2 &texCoords, const glm::vec4 &color, float texIndex)
			: Position(position),
			  TexCoords(texCoords),
			  Color(color),
			  TexIndex(texIndex)
		{
		}

		NX_API static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout = {{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														  {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
														  {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"},
														  {Nexus::Graphics::ShaderDataType::Float, "TEXCOORD"}};
			return layout;
		}
	};

	struct BatchInfo
	{
		Nexus::Ref<Nexus::Graphics::Pipeline>	 Pipeline	 = nullptr;
		Nexus::Ref<Nexus::Graphics::ResourceSet> ResourceSet = nullptr;

		std::vector<Nexus::Graphics::VertexPositionTexCoordColorTexIndex> Vertices;
		std::vector<uint32_t>											  Indices;
		std::vector<Nexus::Ref<Nexus::Graphics::Texture2D>>				  Textures;

		uint32_t ShapeCount	 = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount	 = 0;

		Nexus::Ref<Nexus::Graphics::VertexBuffer> VertexBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::IndexBuffer>  IndexBuffer  = nullptr;
	};

	class BatchRenderer
	{
	  public:
		NX_API BatchRenderer(Nexus::Graphics::GraphicsDevice *device, Nexus::Graphics::RenderTarget target);

		NX_API void Resize();

		NX_API void Begin(Viewport viewport, Scissor scissor);
		NX_API void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color);
		NX_API void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture2D> texture);
		NX_API void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, Ref<Texture2D> texture, float tilingFactor);
		NX_API void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color);
		NX_API void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture2D> texture);
		NX_API void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, Ref<Texture2D> texture, float tilingFactor);
		NX_API void DrawQuad(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, float thickness);
		NX_API void DrawQuad(const Rectangle<float> &rectangle, const glm::vec4 &color, float thickness);
		NX_API void DrawCharacter(char character, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, Font *font);
		NX_API void DrawString(const std::string &text, const glm::vec2 &position, uint32_t size, const glm::vec4 &color, Font *font);
		NX_API void DrawLine(const glm::vec2 &a, const glm::vec2 &b, const glm::vec4 &color, float thickness);
		NX_API void DrawCircle(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, float thickness);
		NX_API void DrawCircle(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, float thickness);
		NX_API void DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints);

		NX_API void DrawCircleRegionFill(const glm::vec2 &position,
										 float			  radius,
										 const glm::vec4 &color,
										 uint32_t		  numberOfPoints,
										 float			  startAngle,
										 float			  fillAngle);

		NX_API void DrawCircleRegionFill(const glm::vec2 &position,
										 float			  radius,
										 const glm::vec4 &color,
										 uint32_t		  numberOfPoints,
										 float			  startAngle,
										 float			  fillAngle,
										 Ref<Texture2D>	  texture);

		NX_API void DrawCircleRegionFill(const glm::vec2 &position,
										 float			  radius,
										 const glm::vec4 &color,
										 uint32_t		  numberOfPoints,
										 float			  startAngle,
										 float			  fillAngle,
										 Ref<Texture2D>	  texture,
										 float			  tilingFactor);

		NX_API void DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture2D> texture);
		NX_API void DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints);
		NX_API void DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, Ref<Texture2D> texture);

		NX_API void DrawCircleFill(const Circle<float> &circle,
								   const glm::vec4	   &color,
								   uint32_t				numberOfPoints,
								   Ref<Texture2D>		texture,
								   float				tilingFactor);

		NX_API void DrawCross(const Rectangle<float> &rectangle, float thickness, const glm::vec4 &color);
		NX_API void DrawTriangle(const glm::vec3 &pos0,
								 const glm::vec2 &uv0,
								 const glm::vec3 &pos1,
								 const glm::vec2 &uv1,
								 const glm::vec3 &pos2,
								 const glm::vec2 &uv2,
								 const glm::vec4 &color);

		NX_API void DrawTriangle(const glm::vec3 &pos0,
								 const glm::vec2 &uv0,
								 const glm::vec3 &pos1,
								 const glm::vec2 &uv1,
								 const glm::vec3 &pos2,
								 const glm::vec2 &uv2,
								 const glm::vec4 &color,
								 Ref<Texture2D>	  texture);

		NX_API void DrawTriangle(const Graphics::Triangle2D &tri, const glm::vec4 &color);
		NX_API void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color);
		NX_API void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture2D> texture);
		NX_API void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, Ref<Texture2D> texture, float tilingFactor);
		NX_API void DrawRoundedRectangle(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, float thickness);
		NX_API void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color);
		NX_API void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, Ref<Texture2D> texture);
		NX_API void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle,
											 const glm::vec4		&color,
											 Ref<Texture2D>			 texture,
											 float					 tilingFactor);
		NX_API void End();

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

		Nexus::Ref<Nexus::Graphics::Texture2D>	   m_BlankTexture  = nullptr;
		Nexus::Ref<Nexus::Graphics::UniformBuffer> m_UniformBuffer = nullptr;

		uint32_t					  m_Width  = 0;
		uint32_t					  m_Height = 0;
		Nexus::Graphics::RenderTarget m_RenderTarget;

		Nexus::Graphics::Viewport m_Viewport;
		Nexus::Graphics::Scissor  m_ScissorRectangle;

		BatchInfo m_TextureBatchInfo;
		BatchInfo m_SDFBatchInfo;
		BatchInfo m_FontBatchInfo;
	};
}	 // namespace Nexus::Graphics