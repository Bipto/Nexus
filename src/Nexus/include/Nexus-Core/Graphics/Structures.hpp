#pragma once

namespace Nexus::Graphics
{
	/// @brief An enum class representing how two pixels should be compared against
	/// each other
	enum class ComparisonFunction
	{
		/// @brief The test will always pass
		AlwaysPass,

		/// @brief The test will pass if the values are equal
		Equal,

		/// @brief The test will pass if the value is greater than the value stored in
		/// the framebuffer
		Greater,

		/// @brief The test will pass if the value is greater or equal to the value
		/// stored in the framebuffer
		GreaterEqual,

		/// @brief The test will pass if the value is lesser than the value stored in
		/// the framebuffer
		Less,

		/// @brief The test will pass if the value is lesser than or equal to the
		/// value stored in the framebuffer
		LessEqual,

		/// @brief The test will never pass
		Never,

		/// @brief The test will pass if the values are not equal
		NotEqual
	};

	/// @brief An enum class representing how two values should be blended together
	enum class BlendFactor
	{
		/// @brief All channels will use a value of zero
		Zero,

		/// @brief All channels will use a value of one
		One,

		/// @brief All channels will use the source colour
		SourceColor,

		/// @brief All channels will use the inverse of the source data (1 - RGB)
		OneMinusSourceColor,

		/// @brief All channels will use the destination colour
		DestinationColor,

		/// @brief All channels will use the inverse of the destination data (1 - RGB)
		OneMinusDestinationColor,

		/// @brief All channels will use the alpha channel of the source
		SourceAlpha,

		/// @brief All channels will use the inverse of the alpha data (1 - A)
		OneMinusSourceAlpha,

		/// @brief All channels will use the alpha channel of the destination
		DestinationAlpha,

		/// @brief All channels will use the inverse of the desination alpha (1 - A)
		OneMinusDestinationAlpha,
	};

	/// @brief A struct representing a mask of pixels to be written
	struct WriteMask
	{
		/// @brief Whether the red channel can be written to
		bool Red = true;

		/// @brief Whether the green channel can be written to
		bool Green = true;

		/// @brief Whether the blue channel can be written to
		bool Blue = true;

		/// @brief Whether the alpha channel can be written to
		bool Alpha = true;
	};

	/// @brief An enum class representing how values will be blended
	enum class BlendEquation
	{
		/// @brief Both values will be added together
		Add,

		/// @brief Subtract source 1 from source 2
		Subtract,

		/// @brief Subtract source 2 from source 1
		ReverseSubtract,

		/// @brief Find the minimum of both values
		Min,

		/// @brief Find the maximum of both values
		Max
	};

	/// @brief An enum class representing how faces will be culled
	enum class CullMode
	{
		/// @brief Back faces will not be rendered
		Back,

		/// @brief Front faces will not be rendered
		Front,

		/// @brief All faces will be rendered
		CullNone
	};

	/// @brief An enum class representing how faces will be filled in
	enum class FillMode
	{
		/// @brief All pixels within the face will be filled
		Solid,

		/// @brief Only the border of the face will be filled
		Wireframe
	};

	/// @brief An enum representing which face should be recognised as the front
	/// face of a mesh
	enum class FrontFace
	{
		/// @brief Polygons with vertices wound clockwise will be set as the front
		/// face
		Clockwise,

		/// @brief Polygons with vertices wound counter-clockwise will be set as the
		/// front face
		CounterClockwise
	};

	/// @brief An enum representing how vertex data is stored within a vertex buffer
	enum class Topology
	{
		/// @brief Each set of two vertices represent an unconnected line
		LineList,

		/// @brief Each vertex is connected to the previous and next vertex in the
		/// buffer
		LineStrip,

		/// @brief Each vertex represents a separate point
		PointList,

		/// @brief Each set of three vertices represent an unconnected face
		TriangleList,

		/// @brief Vertices are shared and every three vertices within the buffer
		/// represents a face (e.g. a vertex buffer with four vertices represents two
		/// triangles)
		TriangleStrip,

		/// @brief Vertices are separate patch primitives
		PatchList
	};

	/// @brief An enum representing how data should be stored when a stencil test is
	/// carried out
	enum class StencilOperation
	{
		/// @brief The value stored in the stencil buffer will be preserved
		Keep,

		/// @brief The value stored in the stencil buffer will be set to zero
		Zero,

		/// @brief The value stored in the stencil buffer will be replaced by the
		/// value produced by the stencil function
		Replace,

		/// @brief The value stored in the stencil buffer will be increased by one if
		/// it is lower than the maximum value
		Increment,

		/// @brief The value stored in the stencil buffer will be decremented by one
		/// if it is higher than the minimum value
		Decrement,

		/// @brief The value stored in the stencil buffer will be inverted
		Invert
	};

	/// @brief A struct representing how the depth and stencil configuration should
	/// be used in a pipeline
	struct DepthStencilDescription
	{
		/// @brief Whether the pipeline can write to the depth buffer
		bool EnableDepthWrite = false;

		/// @brief Whether the pipeline will test pixels against the depth buffer
		bool EnableDepthTest = false;

		/// @brief How the pixel should be tested against the depth buffer
		ComparisonFunction DepthComparisonFunction = ComparisonFunction::AlwaysPass;

		/// @brief Whether the pipeline will use a stencil test when rendering
		bool EnableStencilTest = false;

		/// @brief The mask to use when writing values to the stencil buffer
		uint8_t StencilMask = 0xFF;

		/// @brief How the pixel should be tested against the stencil buffer
		ComparisonFunction StencilComparisonFunction = ComparisonFunction::AlwaysPass;

		/// @brief The value that should be entered in the stencil buffer when the
		/// stencil test fails
		StencilOperation StencilFailOperation = StencilOperation::Keep;

		/// @brief The value that should be entered in the stencil buffer when the
		/// stencil test is successful but the depth test fails
		StencilOperation StencilSuccessDepthFailOperation = StencilOperation::Keep;

		/// @brief The value that should be entered in the stencil buffer when the
		/// stencil test is successful and the depth test is successful
		StencilOperation StencilSuccessDepthSuccessOperation = StencilOperation::Keep;

		/// @brief The value to use for the minimum depth
		float MinDepth = 0.0f;

		/// @brief The value to use for the maximum depth
		float MaxDepth = 1.0f;
	};

	/// @brief A struct representing how triangles should be rendered onto the
	/// screen
	struct RasterizerStateDescription
	{
		/// @brief How triangles should be culled during rendering
		CullMode TriangleCullMode = CullMode::Back;

		/// @brief How triangles should be filled in during rendering
		FillMode TriangleFillMode = FillMode::Solid;

		/// @brief Which faces should be recognised as the front of a mesh
		FrontFace TriangleFrontFace = FrontFace::Clockwise;

		/// @brief Whether the values of the depth buffer should be limited
		bool DepthClipEnabled = false;
	};

	/// @brief A struct represenging how pixels should be blended
	struct BlendStateDescription
	{
		/// @brief Whether blending should be enabled during compositing
		bool EnableBlending = false;

		/// @brief How to perform blending on the output of the source colour
		BlendFactor SourceColourBlend = BlendFactor::SourceColor;

		/// @brief How to perform blending on the output of the source alpha
		BlendFactor SourceAlphaBlend = BlendFactor::One;

		/// @brief How to perform blending on the output of the destination colour
		BlendFactor DestinationColourBlend = BlendFactor::DestinationColor;

		/// @brief How to perform blending on the output of the destination alpha
		BlendFactor DestinationAlphaBlend = BlendFactor::DestinationAlpha;

		/// @brief How the two RGB colour values should be blended together
		BlendEquation ColorBlendFunction = BlendEquation::Add;

		/// @brief How the two alpha value should be blended together
		BlendEquation AlphaBlendFunction = BlendEquation::Add;

		/// @brief How the pixel should be written to the render target
		WriteMask PixelWriteMask = WriteMask {};
	};
}	 // namespace Nexus::Graphics
