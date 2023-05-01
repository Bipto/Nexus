#pragma once

#include <vector>

namespace Nexus::FileDialogs
{
    const char* OpenFile(std::vector<const char*> filters);
    const char* OpenFolder(const char* title, const char* defaultDirectory);
}