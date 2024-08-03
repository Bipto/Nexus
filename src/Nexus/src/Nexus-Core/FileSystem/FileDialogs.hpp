#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus::FileDialogs
{
    /// @brief A method to open a new open dialog, blocking the existing application
    /// @param filters A set of filters to use to limit the files that can be chosen
    /// @return A string containing the filepath
    const char *OpenFile(std::vector<const char *> filters);

    /// @brief A method to open a new open folder dialog, blocking the existing application
    /// @param title  The title of the dialog
    /// @param defaultDirectory The default directory to open the browser in
    /// @return A string containing the path to the folder
    const char *OpenFolder(const char *title, const char *defaultDirectory);
}