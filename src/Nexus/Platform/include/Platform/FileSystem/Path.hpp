#pragma once

#include <filesystem>

#include "Platform/Platform-Core.hpp"
#include "Platform/Timings/DateTime.hpp"

namespace Nexus::IO
{
	enum class PathType
	{
		Invalid,
		File,
		Directory,
		Other
	};

	struct NX_PLATFORM_API PathInfo
	{
		PathType Type		 = {};
		uint64_t SizeInBytes = {};
		DateTime CreatedAt	 = {};
		DateTime ModifiedAt	 = {};
		DateTime AccessedAt	 = {};
	};

	enum class UserFolder
	{
		Home,
		Desktop,
		Documents,
		Downloads,
		Music,
		Pictures,
		PublicShare,
		SavedGames,
		Screenshots,
		Teplates,
		Videos
	};

	class NX_PLATFORM_API Path
	{
	  public:
		/// @brief Gets information about the specified path
		/// @param path The path to get information about
		/// @return A PathInfo structure containing information about the path
		static PathInfo GetPathInfo(const std::filesystem::path &path);

		static std::string GetBasePath();

		static std::string GetApplicationPath(const char *org, const char *app);

		static std::string GetCurrentExecutableDirectory();

		static std::string GetCurrentUserFolder(IO::UserFolder folder);

		static std::string RemovePath(const char *path);

		static std::string RenamePath(const char *oldPath, const char *newPath);
	};

}	 // namespace Nexus::IO