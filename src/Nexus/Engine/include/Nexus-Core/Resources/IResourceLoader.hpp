#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
    /// @brief A class providing an interface for resource loading, e.g. from file or
    /// from a binary blob
    class NX_API IResourceLoader
    {
      public:
        /// @brief Defaulted virtual constructor for resource cleanup
        virtual ~IResourceLoader() = default;

        /// @brief A method that returns the bytes representing a resource from a
        /// given path
        /// @param path The path that the resource should be loaded from
        /// @return A buffer containing the resource, or an error message
        virtual std::expected<std::vector<std::byte>, std::string> LoadBytes(
            std::string_view path
        ) const = 0;

        /// @brief A method that returns a string that was retrieved from the
        /// resource at the given path
        /// @param path The path that the resource should be loaded from
        /// @return A string containing the resource or an error message
        virtual std::expected<std::string, std::string> LoadString(
            std::string_view path
        ) const = 0;

        /// @brief A method that checks if a given file path is accessible to the
        /// resource loader
        /// @param path The path to check
        /// @return Whether the file was found within the resource loader's
        /// filesystem
        virtual bool DoesFileExist(std::string_view path) const = 0;
    };
} // namespace Nexus