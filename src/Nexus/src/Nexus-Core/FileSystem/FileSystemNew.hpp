#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::FileSystemNew
{
	void Initialise();

	void		WriteStringToFile(const std::string &data, const std::string &filepath);
	std::string ReadStringFromFile(const std::string &filepath);

	void			  WriteBufferToFile(void *data, size_t size, const std::string &filepath);
	std::vector<char> ReadBufferFromFile(const std::string &filepath);

	bool IsWritingFilesSupported();
	bool DoesFileExist(const std::string &filepath);
	bool DoesDirectoryExist(const std::string &directory);
}	 // namespace Nexus::FileSystemNew