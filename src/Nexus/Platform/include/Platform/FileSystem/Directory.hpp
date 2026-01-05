#pragma once

#include <string>
#include <vector>

#include "Platform/Platform-Core.hpp"

namespace Nexus::IO
{
	class NX_PLATFORM_API Directory
	{
	  public:
		/// @brief Creates a directory at the specified path
		/// @param path The path to create the directory at
		/// @return An error message if the operation failed, or an empty string on success
		static std::string CreateDirectoryAt(const char *path);

		/// @brief Enumates contents of a directory
		/// @param path The path of the directory to enumerate
		/// @return A vector of strings containing the paths of the contents of the directory
		static std::vector<std::string> EnumerateDirectoryContents(const char *path);
	};
}	 // namespace Nexus::IO