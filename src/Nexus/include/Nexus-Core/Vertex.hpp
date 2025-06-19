#pragma once
#include "Nexus-Core/Graphics/ShaderDataType.hpp"
#include "glm/glm.hpp"

namespace Nexus::Graphics
{
	/// @brief A struct that represents an item within a vertex buffer
	struct NX_API VertexBufferElement
	{
		/// @brief The name of the element, needs to match the name in the shader
		std::string Name;

		/// @brief The type of the element
		ShaderDataType Type;

		/// @brief The total size of the element
		uint32_t Size = 0;

		/// @brief The offset of the element within each separate buffer item
		size_t Offset = 0;

		/// @brief A default constructor to create an empty element
		VertexBufferElement() = default;

		/// @brief A constructor creating a description of a vertex buffer element
		/// @param type The type of the data within the buffer
		/// @param name The name of the item within the buffer
		/// @param normalized Whether the data is normalized
		VertexBufferElement(ShaderDataType type, const std::string &name);

		/// @brief A method that returns an unsigned 32 bit integer representing the
		/// number of components within the element
		/// @return The number of components within the element (e.g. Float2 will
		/// return 2)
		uint32_t GetComponentCount() const;
	};

	enum class StepRate
	{
		Vertex,
		Instance
	};

	/// @brief A struct representing a set of vertex elements stored within a vertex
	/// buffer
	struct NX_API VertexBufferLayout
	{
	  public:
		/// @brief A default constructor creating an empty vertex buffer layout
		VertexBufferLayout() = default;

		/// @brief A constructor taking in an initializer list of vertex buffer
		/// elements to use to create the layout
		/// @param elements An initializer list of vertex buffer elements
		VertexBufferLayout(std::initializer_list<VertexBufferElement> elements, StepRate stepRate) : m_Elements(elements), m_StepRate(stepRate)
		{
			CalculateOffsets();
		}

		/// @brief An iterator returning the beginning of the layout
		/// @return An iterator returning the first element of the layout
		std::vector<VertexBufferElement>::iterator begin()
		{
			return m_Elements.begin();
		}

		/// @brief An iterator returning the end of the layout
		/// @return An iterator returning the last element of the layout
		std::vector<VertexBufferElement>::iterator end()
		{
			return m_Elements.end();
		}

		/// @brief An iterator returning the beginning of the layout
		/// @return An iterator returning the first element of the layout
		std::vector<VertexBufferElement>::const_iterator begin() const
		{
			return m_Elements.begin();
		}

		/// @brief An iterator returning the end of the layout
		/// @return An iterator returning the last element of the layout
		std::vector<VertexBufferElement>::const_iterator end() const
		{
			return m_Elements.end();
		}

		/// @brief A method that returns the number of elements stored within the
		/// layout
		/// @return An unsigned 32 bit integer representing the number of elements
		/// within the layout
		const uint32_t GetNumberOfElements() const
		{
			return m_Elements.size();
		}

		const VertexBufferElement &GetElement(uint32_t index) const
		{
			return m_Elements.at(index);
		}

		bool IsVertexBuffer() const;

		bool IsInstanceBuffer() const;

		uint32_t GetInstanceStepRate() const
		{
			return 1;
		}

	  private:
		/// @brief A private method that calculates the offset of each element within
		/// the buffer
		void CalculateOffsets();

	  private:
		/// @brief A vector containing the elements within the vertex buffer
		std::vector<VertexBufferElement> m_Elements = {};

		/// @brief An enum representing how the data should be iterated
		StepRate m_StepRate = {};
	};

	/// @brief A struct representing a vertex with 3D position in world space
	struct VertexPosition
	{
		/// @brief 3 floating point values representing position of the vertex
		glm::vec3 Position = {0, 0, 0};

		VertexPosition() = default;

		/// @brief A constructor taking in the position of the vertex
		/// @param position A const reference to 3 floating point values representing
		/// the position
		VertexPosition(const glm::vec3 &position) : Position(position)
		{
		}

		/// @brief A static method that returns the vertex buffer layout of this
		/// vertex type
		/// @return A VertexBufferLayout object containing the buffer layout of the
		/// type
		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			/* Nexus::Graphics::VertexBufferLayout layout =
				Nexus::Graphics::VertexBufferLayout({Nexus::Graphics::VertexBufferElement(Nexus::Graphics::ShaderDataType::Float3)},
													StepRate::Vertex); */

			Nexus::Graphics::VertexBufferLayout layout = {{{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}}, StepRate::Vertex};

			return layout;
		}
	};

	/// @brief A struct representing a vertex with 3D position in world space and a
	/// texture coordinate
	struct VertexPositionTexCoord
	{
		/// @brief 3 floating point values representing position of the vertex
		glm::vec3 Position = {0, 0, 0};

		/// @brief 2 floating point values representing the texture coordinates of the
		/// vertex
		glm::vec2 TexCoords = {0, 0};

		VertexPositionTexCoord() = default;

		/// @brief A constructor taking in the position of the vertex and its texture
		/// coordinate
		/// @param position A const reference to 3 floating point values representing
		/// the position
		/// @param texCoords A const reference to 2 floating point values representing
		/// the texture coordinates
		VertexPositionTexCoord(const glm::vec3 &position, const glm::vec2 &texCoords) : Position(position), TexCoords(texCoords)
		{
		}

		/// @brief A static method that returns the vertex buffer layout of this
		/// vertex type
		/// @return A VertexBufferLayout object containing the buffer layout of the
		/// type
		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout = {
				{{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}, {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"}},
				StepRate::Vertex};
			return layout;
		}
	};

	/// @brief A struct representing a vertex with 3D position in world space, a
	/// texture coordinate and a color
	struct VertexPositionTexCoordColor
	{
		/// @brief 3 floating point values representing position of the vertex
		glm::vec3 Position = {0, 0, 0};

		/// @brief 2 floating point values representing the texture coordinates of the
		/// vertex
		glm::vec2 TexCoords = {0, 0};

		// @brief 4 floating point values representing the colour of the vertex
		glm::vec4 Color = {1.0f, 1.0f, 1.0f, 1.0f};

		VertexPositionTexCoordColor() = default;

		/// @brief A constructor taking in the position of the vertex and its texture
		/// coordinate
		/// @param position A const reference to 3 floating point values representing
		/// the position
		/// @param texCoords A const reference to 2 floating point values representing
		/// the texture coordinates
		/// @param color A const reference to 4 floating point values repsenting the
		/// colour
		VertexPositionTexCoordColor(const glm::vec3 &position, const glm::vec2 &texCoords, const glm::vec4 &color)
			: Position(position),
			  TexCoords(texCoords),
			  Color(color)
		{
		}

		/// @brief A static method that returns the vertex buffer layout of this
		/// vertex type
		/// @return A VertexBufferLayout object containing the buffer layout of the
		/// type
		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout = {{{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"}},
														  StepRate::Vertex};
			return layout;
		}
	};

	/// @brief A struct representing a vertex with 3D position in world space, a
	/// texture coordinate and a normal
	struct VertexPositionTexCoordNormal
	{
		/// @brief 3 floating point values representing position of the vertex
		glm::vec3 Position = {0, 0, 0};

		/// @brief 2 floating point values representing the texture coordinates of the
		/// vertex
		glm::vec2 TexCoords = {0, 0};

		/// @brief 3 floating point values representing the normal of the vertex
		glm::vec3 Normal = {0, 0, 0};

		VertexPositionTexCoordNormal() = default;

		/// @brief A constructor taking in the position of the vertex, it's texture
		/// coordinate and it's normal
		/// @param position A const reference to 3 floating point values representing
		/// the position
		/// @param texCoords A const reference to 2 floating point values representing
		/// the texture coordinates
		/// @param normal A const reference to 3 floating point values representing
		/// the normal
		VertexPositionTexCoordNormal(const glm::vec3 &position, const glm::vec2 &texCoords, const glm::vec3 &normal)
			: Position(position),
			  TexCoords(texCoords),
			  Normal(normal)
		{
		}

		/// @brief A static method that returns the vertex buffer layout of this
		/// vertex type
		/// @return A VertexBufferLayout object containing the buffer layout of the
		/// type
		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout = {{{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}},
														  StepRate::Vertex};
			return layout;
		}
	};

	/// @brief A struct representing a vertex with 3D position in world space, a
	/// texture coordinate, normal, tangent and binormal
	struct VertexPositionTexCoordNormalTangentBitangent
	{
		/// @brief 3 floating point values representing position of the vertex
		glm::vec3 Position = {0, 0, 0};

		/// @brief 2 floating point values representing the texture coordinates of the
		/// vertex
		glm::vec2 TexCoords = {0, 0};

		/// @brief 3 floating point values representing the normal of the vertex
		glm::vec3 Normal = {0, 0, 0};

		/// @brief 3 floating point values representing the tangent of the vertex
		glm::vec3 Tangent = {0, 0, 0};

		/// @brief 3 floating point values representing the binormal of the vertex
		glm::vec3 Bitangent = {0, 0, 0};

		VertexPositionTexCoordNormalTangentBitangent() = default;

		/// @brief A constructor taking in the position of the vertex, it's texture
		/// coordinate and it's normal
		/// @param position A const reference to 3 floating point values representing
		/// the position
		/// @param texCoords A const reference to 2 floating point values representing
		/// the texture coordinates
		/// @param normal A const reference to 3 floating point values representing
		/// the normal
		VertexPositionTexCoordNormalTangentBitangent(const glm::vec3 &position,
													 const glm::vec2 &texCoords,
													 const glm::vec3 &normal,
													 const glm::vec3 &tangent,
													 const glm::vec3 &bitangent)
			: Position(position),
			  TexCoords(texCoords),
			  Normal(normal),
			  Tangent(tangent),
			  Bitangent(bitangent)
		{
		}

		/// @brief A static method that returns the vertex buffer layout of this
		/// vertex type
		/// @return A VertexBufferLayout object containing the buffer layout of the
		/// type
		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout = {{{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}},
														  StepRate::Vertex};
			return layout;
		}
	};

	/// @brief A struct representing a vertex with 3D position in world space, a
	/// texture coordinate, normal, tangent and binormal
	struct VertexPositionTexCoordNormalColourTangentBitangent
	{
		/// @brief 3 floating point values representing position of the vertex
		glm::vec3 Position = {0, 0, 0};

		/// @brief 2 floating point values representing the texture coordinates of the
		/// vertex
		glm::vec2 TexCoords = {0, 0};

		/// @brief 3 floating point values representing the normal of the vertex
		glm::vec3 Normal = {0, 0, 0};

		/// @brief  4 floating point values representing the colour of the vertex
		glm::vec4 Colour = {1, 1, 1, 1};

		/// @brief 3 floating point values representing the tangent of the vertex
		glm::vec3 Tangent = {0, 0, 0};

		/// @brief 3 floating point values representing the binormal of the vertex
		glm::vec3 Bitangent = {0, 0, 0};

		VertexPositionTexCoordNormalColourTangentBitangent() = default;

		/// @brief A constructor taking in the position of the vertex, it's texture
		/// coordinate and it's normal
		/// @param position A const reference to 3 floating point values representing
		/// the position
		/// @param texCoords A const reference to 2 floating point values representing
		/// the texture coordinates
		/// @param normal A const reference to 3 floating point values representing
		/// the normal
		VertexPositionTexCoordNormalColourTangentBitangent(const glm::vec3 &position,
														   const glm::vec2 &texCoords,
														   const glm::vec3 &normal,
														   const glm::vec4 &colour,
														   const glm::vec3 &tangent,
														   const glm::vec3 &bitangent)
			: Position(position),
			  TexCoords(texCoords),
			  Normal(normal),
			  Colour(colour),
			  Tangent(tangent),
			  Bitangent(bitangent)
		{
		}

		/// @brief A static method that returns the vertex buffer layout of this
		/// vertex type
		/// @return A VertexBufferLayout object containing the buffer layout of the
		/// type
		static Nexus::Graphics::VertexBufferLayout GetLayout()
		{
			Nexus::Graphics::VertexBufferLayout layout = {{{Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float2, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float4, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"},
														   {Nexus::Graphics::ShaderDataType::Float3, "TEXCOORD"}},
														  StepRate::Vertex};
			return layout;
		}
	};

	namespace Utilities
	{
		NX_API std::vector<VertexPositionTexCoordNormalTangentBitangent> GenerateTangentAndBinormals(
			const std::vector<Nexus::Graphics::VertexPositionTexCoordNormal> &vertices);
	}
}	 // namespace Nexus::Graphics