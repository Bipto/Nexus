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

namespace Nexus
{
	FileResourceLoader::FileResourceLoader(std::string_view directory) : m_Directory(directory)
	{
	}

	std::expected<std::vector<std::byte>, std::string> FileResourceLoader::Load(std::string_view path) const
	{
		try
		{
			std::filesystem::path base	   = std::filesystem::weakly_canonical(m_Directory);
			std::filesystem::path resolved = std::filesystem::weakly_canonical(base / path);

			// Check if resolved is inside base
			auto rel = resolved.lexically_relative(base);

			// If rel is empty → paths are identical → not a file
			// If rel begins with ".." → outside base directory
			if (rel.empty() || rel.begin()->native() == L"..")
			{
				return std::unexpected("Attempt to access file outside base directory");
			}

			// Now load the file
			return IO::File::ReadAllBytes(resolved.string());
		}
		catch (const std::exception &e)
		{
			return std::unexpected(e.what());
		}
	}

}	 // namespace Nexus
