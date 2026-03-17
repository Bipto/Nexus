#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief A class providing an interface for resource loading, e.g. from file or from a binary blob
	class NX_API IResourceLoader
	{
	  public:
		/// @brief Defaulted virtual constructor for resource cleanup
		virtual ~IResourceLoader() = default;

		/// @brief A method that returns the bytes representing a resource from a given path
		/// @param path The path that the resource should be loaded from
		/// @return A buffer containing the resource, or an error message
		virtual std::expected<std::vector<std::byte>, std::string> Load(std::string_view path) const = 0;
	};
}	 // namespace Nexus