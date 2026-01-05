#include "Platform/FileSystem/Path.hpp"
#include "Platform/Platform.hpp"

namespace Nexus::IO
{
	PathInfo Path::GetPathInfo(const std::filesystem::path &path)
	{
		return Platform::GetPathInfo(path.string().c_str());
	}

	std::string Path::GetBasePath()
	{
		return Platform::GetRootPath();
	}

	std::string Path::GetApplicationPath(const char *org, const char *app)
	{
		return Platform::GetApplicationPath(org, app);
	}

	std::string Path::GetCurrentExecutableDirectory()
	{
		return Platform::GetCurrentExecutableDirectory();
	}

	std::string Path::GetCurrentUserFolder(UserFolder folder)
	{
		return Platform::GetCurrentUserFolder(folder);
	};

	std::string Path::RemovePath(const char *path)
	{
		return Platform::RemovePath(path);
	}

	std::string Path::RenamePath(const char *oldPath, const char *newPath)
	{
		return Platform::RenamePath(oldPath, newPath);
	}
}	 // namespace Nexus::IO