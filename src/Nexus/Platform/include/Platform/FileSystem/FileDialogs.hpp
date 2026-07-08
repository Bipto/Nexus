#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Platform/Platform-Core.hpp"

namespace Nexus
{
    class IWindow;

    struct FileDialogFilter
    {
        const char *Name;
        const char *Pattern;
    };

    struct OpenFileDialogDescription
    {
        IWindow *WindowHandle = nullptr;
        std::vector<FileDialogFilter> Filters = {};
        std::optional<std::string> DefaultLocation = {};
        bool AllowMany = false;
        std::string TitleString = "Open File";
        std::string AcceptString = "Open";
        std::string CancelString = "Cancel";
    };

    struct SaveFileDialogDescription
    {
        IWindow *WindowHandle = nullptr;
        std::vector<FileDialogFilter> Filters = {};
        std::optional<std::string> DefaultLocation = {};
        std::string TitleString = "Save File";
        std::string AcceptString = "Save";
        std::string CancelString = "Cancel";
    };

    struct OpenFolderDialogDescription
    {
        IWindow *WindowHandle = nullptr;
        std::vector<FileDialogFilter> Filters = {};
        std::optional<std::string> DefaultLocation = {};
        bool AllowMany = false;
        std::string TitleString = "Open Folder";
        std::string AcceptString = "Open";
        std::string CancelString = "Cancel";
    };

    struct FileDialogResult
    {
        std::vector<std::string> FilePaths;
        int SelectedFilter = -1;
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

} // namespace Nexus