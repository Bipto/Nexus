#pragma once

#include "Core/ResourceHandle.hpp"
#include <cstdint>

namespace Nexus
{
	/// @brief A class representing a collection of resources, identified by handles
	/// @tparam T A template parameter of the resource to store in the pool
	/// @tparam Handle A template type of a handle type to identify the resource
	template<typename T, typename Handle>
	class ResourcePool
	{
	  public:
		/// @brief A structure representing a stored resource within the pool
		struct Entry
		{
			/// @brief A template type of the resource
			T resource {};

			/// @brief An integer representing the generation that the resource belongs to
			uint32_t generation = 0;

			/// @brief A boolean indicating whether this resource entry is available
			bool alive = false;
		};

		/// @brief A method that enters an existing resource into the resource pool
		/// @param resource A reference to the resource to copy into the pool
		/// @return A handle representing the stored resource
		Handle Create(const T &resource);

		/// @brief A method that constructs a resource in-place within the pool
		/// @tparam ...Args A template of the function parameter types
		/// @param ...args The parameter values to be use to construct the object
		/// @return A handle representing the stored resource
		template<typename... Args>
		Handle Emplace(Args &&...args);

		/// @brief A method that releases an object from within the resource pool
		/// @param handle The handle of the resource to be freed
		void Destroy(Handle handle);

		/// @brief A method that retrieves a pointer to the resource from within the resource pool
		/// @param handle The handle of the resource to be retrieved
		/// @return A pointer to the underlying resource
		T *Get(Handle handle);

	  private:
		/// @brief A vector containing all stored entries within the resource pool
		std::vector<Entry> m_Entries = {};

		/// @brief A vector containing all resource slots that have been marked as avaible for re-use
		std::vector<uint64_t> m_FreeList = {};
	};
}	 // namespace Nexus

#include "Core/ResourcePool.inl"