#include <cstddef>
#include <exception>
#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "Nexus-Core/Resources/FileResourceLoader.hpp"
#include "Nexus-Core/Resources/IResourceLoader.hpp"

#include "Platform/FileSystem/File.hpp"
#include "Platform/FileSystem/Path.hpp"

namespace Nexus
{
	FileResourceLoader::FileResourceLoader(std::string_view directory) : m_Directory(directory)
	{
	}

	static std::expected<std::string, std::string> ResolveFilepath(std::string_view path, const std::filesystem::path &directory)
	{
		std::filesystem::path base	   = std::filesystem::weakly_canonical(directory);
		std::filesystem::path resolved = std::filesystem::weakly_canonical(base / path);

		// Check if resolved is inside base
		auto rel = resolved.lexically_relative(base);

		// If rel is empty → paths are identical → not a file
		// If rel begins with ".." → outside base directory
		if (rel.empty() || *rel.begin() == std::filesystem::path(".."))
		{
			return std::unexpected("Attempt to access file outside base directory");
		}

		return resolved.string();
	}

	std::expected<std::vector<std::byte>, std::string> FileResourceLoader::LoadBytes(std::string_view path) const
	{
		try
		{
			// resolve the path and then attempt to read the contents
			return ResolveFilepath(path, m_Directory).and_then(IO::File::ReadAllBytes);
		}
		catch (const std::exception &e)
		{
			return std::unexpected(e.what());
		}
	}

	std::expected<std::string, std::string> FileResourceLoader::LoadString(std::string_view path) const
	{
		try
		{
			// resolve the path and then attempt to read the contents
			return ResolveFilepath(path, m_Directory).and_then(IO::File::ReadAllText);
		}
		catch (const std::exception &e)
		{
			return std::unexpected(e.what());
		}
	}

	bool FileResourceLoader::DoesFileExist(std::string_view path) const
	{
		auto resolvedPath = ResolveFilepath(path, m_Directory);
		if (!resolvedPath)
		{
			return false;
		}

		auto pathInfo = IO::Path::GetPathInfo(std::filesystem::path {*resolvedPath});
		return pathInfo.Type == IO::PathType::File;
	}

}	 // namespace Nexus
