#pragma once

#include <cstddef>
#include <exception>
#include <expected>
#include <filesystem>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/Resources/IResourceLoader.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	/// @brief A class providing an implementation of an interface for loading resources from memory
	class NX_API MemoryResourceLoader : public IResourceLoader
	{
	  public:
		/// @brief A defaulted constructor to create a new instance of MemoryResourceLoader
		MemoryResourceLoader() = default;

		/// @brief Defaulted virtual constructor for resource cleanup
		~MemoryResourceLoader() final = default;

		/// @brief A method that returns the bytes representing a resource from a given path
		/// @param path The path that the resource should be loaded from
		/// @return A buffer containing the resource, or an error message
		virtual std::expected<std::vector<std::byte>, std::string> Load(std::string_view path) const final;

		/// @brief A method that adds data into this resource loader and is indexed with the given file path
		/// @param path The path that the data should be mounted at
		/// @param overwrite Specifies whether the loader should give an error if a file is already mounted at the given address, if overwrite is
		/// true, the original data will be replaced by the supplied data
		/// @param data The data that should be stored within this loader
		std::expected<void, std::string> MountFile(std::string_view path, bool overwrite, const std::vector<std::byte> &data);

	  private:
		/// @brief A mutex to control access to the resources across threads
		mutable std::shared_mutex m_Mutex = {};

		/// @brief The base path that resources will be attempted to load from
		std::map<std::string, std::vector<std::byte>> m_MountedFiles = {};
	};
}	 // namespace Nexus
