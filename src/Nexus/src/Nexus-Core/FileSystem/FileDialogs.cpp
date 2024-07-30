#include "FileDialogs.hpp"

#if defined(WIN32)
#include "tinyfiledialogs.h"
#include "more_dialogs/tinyfd_moredialogs.h"
#endif

namespace Nexus::FileDialogs
{
    const char *OpenFile(std::vector<const char *> filters)
    {
#if defined(WIN32)
        return tinyfd_openFileDialog(
            "Select a file",
            "",
            filters.size(),
            filters.data(),
            NULL,
            0);
#endif

        return "";
    }

    const char *OpenFolder(const char *title, const char *defaultDirectory)
    {
#if defined(WIN32)
        return tinyfd_selectFolderDialog(
            title,
            defaultDirectory);
#endif
        return "";
    }
}