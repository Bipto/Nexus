#pragma once

#include <string>
#include <vector>

#include "Platform/Platform-Core.hpp"

namespace Nexus
{
	struct FileDialogFilter
	{
		const char *Name;
		const char *Pattern;
	};

	struct FileDialogResult
	{
		std::vector<std::string> FilePaths;
		int						 SelectedFilter = -1;
	};

	class NX_PLATFORM_API OpenFileDialog
	{
	  public:
		virtual ~OpenFileDialog()
		{
		}
		virtual FileDialogResult Show() = 0;
	};

	class NX_PLATFORM_API SaveFileDialog
	{
	  public:
		virtual ~SaveFileDialog()
		{
		}
		virtual FileDialogResult Show() = 0;
	};

	class NX_PLATFORM_API OpenFolderDialog
	{
	  public:
		virtual ~OpenFolderDialog()
		{
		}
		virtual FileDialogResult Show() = 0;
	};

}	 // namespace Nexus