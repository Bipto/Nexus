#pragma once

#include "Nexus-Core/nxpch.hpp"

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

	class OpenFileDialog
	{
	  public:
		NX_API virtual ~OpenFileDialog()
		{
		}
		NX_API virtual FileDialogResult Show() = 0;
	};

	class SaveFileDialog
	{
	  public:
		NX_API virtual ~SaveFileDialog()
		{
		}
		NX_API virtual FileDialogResult Show() = 0;
	};

	class OpenFolderDialog
	{
	  public:
		NX_API virtual ~OpenFolderDialog()
		{
		}
		NX_API virtual FileDialogResult Show() = 0;
	};

}	 // namespace Nexus