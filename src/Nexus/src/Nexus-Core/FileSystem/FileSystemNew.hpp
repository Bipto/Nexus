#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::FileSystemNew
{
	enum class FileMode
	{
		Binary,
		Text
	};

	void			  WriteBufferToFile(void *data, size_t size, const std::string &filepath, FileMode mode);
	std::vector<char> ReadBufferFromFile(const std::string &filepath, FileMode mode);
}	 // namespace Nexus::FileSystemNew