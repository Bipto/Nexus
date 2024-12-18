#pragma once

#include "Nexus-Core/Buffer.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Nexus::FileSystemNew
{
	enum class FileMode
	{
		Binary,
		Text
	};

	void		 WriteBufferToFile(Buffer<char> buffer, const std::string &filepath, FileMode mode);
	Buffer<char> ReadBufferFromFile(const std::string &filepath, FileMode mode);
}	 // namespace Nexus::FileSystemNew