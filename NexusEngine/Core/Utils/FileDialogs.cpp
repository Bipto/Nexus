#include "FileDialogs.h"

#include "tinyfiledialogs.h"
#include "more_dialogs/tinyfd_moredialogs.h"

namespace Nexus::FileDialogs
{
    const char *OpenFile(std::vector<const char *> filters)
    {
        return tinyfd_openFileDialog(
            "Select a file",
            "",
            filters.size(),
            filters.data(),
            NULL,
            0
        );
    }

    const char *OpenFolder(const char* title, const char* defaultDirectory)
    {
        return tinyfd_selectFolderDialog(
            title,
            defaultDirectory
        );
    }
}