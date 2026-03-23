#pragma once

#include <cstddef>
#include <exception>
#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/Resources/IResourceLoader.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief A class providing an implementation of an interface for loading resources from disk
	class NX_API FileResourceLoader : public IResourceLoader
	{
	  public:
		/// @brief A constructor to create a new instance of FileResourceLoader
		/// @param directory The base directory that contains the resources to be loaded with this class
		FileResourceLoader(std::string_view directory);

		/// @brief Defaulted virtual constructor for resource cleanup
		~FileResourceLoader() final = default;

		/// @brief A method that returns the bytes representing a resource from a given path
		/// @param path The path that the resource should be loaded from
		/// @return A buffer containing the resource, or an error message
		virtual std::expected<std::vector<std::byte>, std::string> LoadBytes(std::string_view path) const final;

		/// @brief A method that returns a string that was retrieved from the resource at the given path
		/// @param path The path that the resource should be loaded from
		/// @return A string containing the resource or an error message
		virtual std::expected<std::string, std::string> LoadString(std::string_view path) const final;

	  private:
		/// @brief The base path that resources will be attempted to load from
		std::filesystem::path m_Directory = {};
	};
}	 // namespace Nexus
