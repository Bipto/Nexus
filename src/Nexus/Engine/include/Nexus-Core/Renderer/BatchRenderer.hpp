#pragma once

#include "Nexus-Core/Graphics/Circle.hpp"
#include "Nexus-Core/Graphics/Font.hpp"
#include "Nexus-Core/Graphics/Polygon.hpp"
#include "Nexus-Core/Graphics/Rectangle.hpp"
#include "Nexus-Core/Graphics/RoundedRectangle.hpp"
#include "Nexus-Core/Utils/GUID.hpp"
#include "Nexus-Core/Vertex.hpp"
#include "RHI/GraphicsDevice.hpp"

namespace Nexus::Graphics
{
	struct BatchVertex
	{
		glm::vec4		  Colour	= {1.0f, 1.0f, 1.0f, 1.0f};
		glm::vec3		  Position	= {0, 0, 0};
		float			  TexIndex	= 0.0f;
		glm::vec2		  TexCoords = {0, 0};
		Point2D<uint32_t> Id		= {0, 0};

		BatchVertex() = default;

		BatchVertex(const glm::vec3 &position, const glm::vec2 &texCoords, const glm::vec4 &color, float texIndex, Point2D<uint32_t> id)
			: Position(position),
			  TexCoords(texCoords),
			  Colour(color),
			  TexIndex(texIndex),
			  Id(id)
		{
		}

		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout =
				Graphics::VertexBufferLayout({{Nexus::Graphics::ShaderDataType::R32G32B32A32_SFloat, "TEXCOORD"},
											  {Nexus::Graphics::ShaderDataType::R32G32B32_SFloat, "TEXCOORD"},
											  {Nexus::Graphics::ShaderDataType::R32_SFloat, "TEXCOORD"},
											  {Nexus::Graphics::ShaderDataType::R32G32_SFloat, "TEXCOORD"},
											  {Nexus::Graphics::ShaderDataType::R32G32_UInt, "TEXCOORD"}},
											 sizeof(BatchVertex),
											 Graphics::StepRate::Vertex);
			return layout;
		}
	};

	struct BatchInfo
	{
		PipelineHandle	  Pipeline	  = {};
		ResourceSetHandle ResourceSet = {};

		std::vector<Nexus::Graphics::BatchVertex> Vertices;
		std::vector<uint32_t>					  Indices;
		std::vector<TextureViewHandle>			  Textures;

		uint32_t ShapeCount	 = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount	 = 0;

		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> VertexUploadBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> VertexBuffer		  = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> IndexUploadBuffer  = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> IndexBuffer		  = nullptr;
	};

	class NX_API BatchRenderer
	{
	  public:
		BatchRenderer(Nexus::Graphics::IGraphicsDevice *device, Graphics::CommandQueueHandle commandQueue, bool useDepthTest, uint32_t sampleCount);

		void Begin(FramebufferHandle target, Viewport viewport, Scissor scissor);
		void Begin(FramebufferHandle target, Viewport viewport, Scissor scissor, const glm::mat4 &camera);

		void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color);
		void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, TextureViewHandle texture);
		void DrawQuadFill(const glm::vec2 &min, const glm::vec2 &max, const glm::vec4 &color, TextureViewHandle texture, float tilingFactor);
		void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color);
		void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, TextureViewHandle texture);
		void DrawQuadFill(const Rectangle<float> &rectangle, const glm::vec4 &color, TextureViewHandle texture, float tilingFactor);
		void DrawQuadFill(const glm::vec4 &color, TextureViewHandle texture, float tilingFactor, const glm::mat4 &transform, Nexus::GUID id);
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

		void DrawCircleRegionFill(const glm::vec2  &position,
								  float				radius,
								  const glm::vec4  &color,
								  uint32_t			numberOfPoints,
								  float				startAngle,
								  float				fillAngle,
								  TextureViewHandle texture);

		void DrawCircleRegionFill(const glm::vec2  &position,
								  float				radius,
								  const glm::vec4  &color,
								  uint32_t			numberOfPoints,
								  float				startAngle,
								  float				fillAngle,
								  TextureViewHandle texture,
								  float				tilingFactor);

		void DrawCircleFill(const glm::vec2 &position, float radius, const glm::vec4 &color, uint32_t numberOfPoints, TextureViewHandle texture);
		void DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints);
		void DrawCircleFill(const Circle<float> &circle, const glm::vec4 &color, uint32_t numberOfPoints, TextureViewHandle texture);

		void DrawCircleFill(const Circle<float> &circle,
							const glm::vec4		&color,
							uint32_t			 numberOfPoints,
							TextureViewHandle	 texture,
							float				 tilingFactor);

		void DrawCross(const Rectangle<float> &rectangle, float thickness, const glm::vec4 &color);
		void DrawTriangle(const glm::vec3 &pos0,
						  const glm::vec2 &uv0,
						  const glm::vec3 &pos1,
						  const glm::vec2 &uv1,
						  const glm::vec3 &pos2,
						  const glm::vec2 &uv2,
						  const glm::vec4 &color);

		void DrawTriangle(const glm::vec3  &pos0,
						  const glm::vec2  &uv0,
						  const glm::vec3  &pos1,
						  const glm::vec2  &uv1,
						  const glm::vec3  &pos2,
						  const glm::vec2  &uv2,
						  const glm::vec4  &color,
						  TextureViewHandle texture);

		void DrawTriangle(const Graphics::Triangle2D &tri, const glm::vec4 &color);
		void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color);
		void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, TextureViewHandle texture);
		void DrawPolygonFill(const Polygon &polygon, const glm::vec4 &color, TextureViewHandle texture, float tilingFactor);
		void DrawRoundedRectangle(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, float thickness);
		void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color);
		void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle, const glm::vec4 &color, TextureViewHandle texture);
		void DrawRoundedRectangleFill(const RoundedRectangle &roundedRectangle,
									  const glm::vec4		 &color,
									  TextureViewHandle		  texture,
									  float					  tilingFactor);
		void End();

	  private:
		void Flush();
		void EnsureStarted();
		void EnsureSpace(BatchInfo &info, uint32_t shapeVertexCount, uint32_t shapeIndexCount);
		void PerformDraw(BatchInfo &info);

	  private:
		Nexus::Graphics::IGraphicsDevice		 *m_Device		 = nullptr;
		Graphics::CommandQueueHandle			  m_CommandQueue = {};
		Nexus::Ref<Nexus::Graphics::ICommandList> m_CommandList	 = nullptr;
		Graphics::SamplerHandle					  m_Sampler		 = {};
		bool									  m_IsStarted	 = false;

		Graphics::TextureHandle					   m_BlankTexture		 = {};
		Graphics::TextureViewHandle				   m_BlankTextureView	 = {};
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_UniformUploadBuffer = nullptr;
		Nexus::Ref<Nexus::Graphics::IDeviceBuffer> m_UniformBuffer		 = nullptr;

		FramebufferHandle m_RenderTarget = {};

		Nexus::Graphics::Viewport m_Viewport		 = {};
		Nexus::Graphics::Scissor  m_ScissorRectangle = {};

		BatchInfo m_TextureBatchInfo = {};
		BatchInfo m_SDFBatchInfo	 = {};
		BatchInfo m_FontBatchInfo	 = {};

		bool m_UseDepthTest = false;
	};
}	 // namespace Nexus::Graphics