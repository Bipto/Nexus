#pragma once

#include <string>
#include <vector>

#include "RHI/RHI-Core.hpp"
#include "RHI/ShaderDataType.hpp"

namespace Nexus::Graphics
{
	/// @brief A struct that represents an item within a vertex buffer
	struct NX_RHI_API VertexBufferElement
	{
		/// @brief The name of the element, needs to match the name in the shader
		std::string Name;

		/// @brief The type of the element
		ShaderDataType Type = ShaderDataType::Invalid;

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
	struct NX_RHI_API VertexBufferLayout
	{
	  public:
		/// @brief A default constructor creating an empty vertex buffer layout
		VertexBufferLayout() = default;

		/// @brief A constructor taking in an initializer list of vertex buffer
		/// elements to use to create the layout
		/// @param elements An initializer list of vertex buffer elements
		VertexBufferLayout(std::initializer_list<VertexBufferElement> elements, size_t stride, StepRate stepRate)
			: m_Elements(elements),
			  m_Stride(stride),
			  m_StepRate(stepRate)
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

		size_t GetStride() const
		{
			return m_Stride;
		}

	  private:
		/// @brief A private method that calculates the offset of each element within
		/// the buffer
		void CalculateOffsets();

	  private:
		/// @brief A vector containing the elements within the vertex buffer
		std::vector<VertexBufferElement> m_Elements = {};

		/// @brief An unsigned integer containing the stride of the vertex buffer
		size_t m_Stride = 0;

		/// @brief An enum representing how the data should be iterated
		StepRate m_StepRate = {};
	};
}	 // namespace Nexus::Graphics