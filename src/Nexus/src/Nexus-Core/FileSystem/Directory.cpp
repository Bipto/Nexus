#include "Nexus-Core/FileSystem/Directory.hpp"
#include "Nexus-Core/Platform.hpp"

namespace Nexus::IO
{
	std::string Directory::CreateDirectoryAt(const char *path)
	{
		return Platform::CreateDirectoryAt(path);
	}
	std::vector<std::string> Directory::EnumerateDirectoryContents(const char *path)
	{
		return Platform::EnumerateDirectoryContents(path);
	}
}	 // namespace Nexus::IO