#include <cstddef>
#include <exception>
#include <expected>
#include <filesystem>
#include <format>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/Resources/IResourceLoader.hpp"
#include "Nexus-Core/Resources/MemoryResourceLoader.hpp"

namespace Nexus
{
	std::expected<std::vector<std::byte>, std::string> MemoryResourceLoader::LoadBytes(std::string_view path) const
	{
		std::string filepath(path);

		// lock the resource for this thread
		std::shared_lock<std::shared_mutex> lock(m_Mutex);

		// check whether the file exists and return the data if it does
		if (m_MountedFiles.contains(filepath))
		{
			return m_MountedFiles.at(filepath);
		}
		// otherwise, return an error to the caller
		else
		{
			return std::unexpected(std::format("Failed to find data mounted to given path: {}", filepath));
		}
	}

	std::expected<std::string, std::string> MemoryResourceLoader::LoadString(std::string_view path) const
	{
		return LoadBytes(path).and_then([](std::vector<std::byte> data) -> std::expected<std::string, std::string>
										{ return std::string(reinterpret_cast<const char *>(data.data()), data.size()); });
	}

	std::expected<void, std::string> MemoryResourceLoader::MountFile(std::string_view path, bool overwrite, const std::vector<std::byte> &data)
	{
		std::string filepath(path);

		// lock the resource for this thread
		std::unique_lock<std::shared_mutex> lock(m_Mutex);

		// check whether the user intened to replace an existing mounted file
		if (m_MountedFiles.contains(filepath) && !overwrite)
		{
			return std::unexpected(
				std::format("Attempting to mount file to path, however a file already exists and overwrite is disabled: {}", filepath));
		}

		// mount the file at the given filepath
		m_MountedFiles[filepath] = data;

		return {};
	}

}	 // namespace Nexus
