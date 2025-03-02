#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief A typedef to simplify using a unique pointer
	/// @tparam T A type to contain within the pointer
	template<typename T>
	using Scope = std::unique_ptr<T>;

	/// @brief A method to return a new Scope
	/// @tparam T A type to contain within the pointer
	/// @tparam ...Args Arguments to provide to the constructor of the object
	/// @param ...args Argument parameters passed to the constructor
	/// @return A unique pointer containing the object
	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args &&...args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	/// @brief A typedef to simplify using a shared pointer
	/// @tparam T A type to contain within the pointer
	template<typename T>
	using Ref = std::shared_ptr<T>;

	/// @brief A method to return a new Ref
	/// @tparam T A type to contain within the pointer
	/// @tparam ...Args Arguments to provide to the constructor of the object
	/// @param ...args Argument parameters passed to the constructor
	/// @return A shared pointer containing the object
	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args &&...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using WeakRef = std::weak_ptr<T>;
}	 // namespace Nexus