#pragma once

#include <cstdint>

namespace Nexus
{
	/// @brief A class representing a unique handle to a resource in a resource pool
	/// @tparam Tag A template type to customise the resource pointed to by the handle
	template<typename Tag>
	struct HandleT
	{
	  public:
		/// @brief A default constructor to initialise an empty object
		HandleT() = default;
		/// @brief The constructor of a resource handle
		/// @param index The index of the resource in a resource pool
		/// @param generation The generation of the resource in the pool (slots can be reused multiple times)
		/// @param parent The parent resource pool of the handle
		HandleT(uint32_t index, uint32_t generation, void *parent);

		/// @brief A method to return the index of the handle
		/// @return An integer representing the index of the handle
		uint32_t GetIndex() const;

		/// @brief A method to return the generation of the handle
		/// @return An integer representing the generation of the handle
		uint32_t GetGeneration() const;

		/// @brief A method that returns a pointer to the parent object of this handle
		/// @return A pointer to the parent object
		void *GetParent() const;

	  private:
		/// @brief An integer containing the index of the handle
		uint32_t m_Index = 0;

		/// @brief An integer containing the generation of the handle
		uint32_t m_Generation = 0;

		/// @brief A pointer to the parent resource pool of the handle
		void *m_Parent = nullptr;
	};

}	 // namespace Nexus

#include "ResourceHandle.inl"