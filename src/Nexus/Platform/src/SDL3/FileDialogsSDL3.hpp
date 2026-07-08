#pragma once

#include "Platform/FileSystem/FileDialogs.hpp"
#include "Platform/IWindow.hpp"
#include "SDL3Include.hpp"

namespace Nexus
{
    class OpenFileDialogSDL3 final : public OpenFileDialog
    {
      public:
        OpenFileDialogSDL3(const OpenFileDialogDescription &desc);
        virtual ~OpenFileDialogSDL3();
        FileDialogResult Show() final;

      private:
        OpenFileDialogDescription m_Description = {};
        std::vector<SDL_DialogFileFilter> m_Filters = {};
    };

    class SaveFileDialogSDL3 final : public SaveFileDialog
    {
      public:
        SaveFileDialogSDL3(const SaveFileDialogDescription &desc);
        virtual ~SaveFileDialogSDL3();
        FileDialogResult Show() final;

      private:
        SaveFileDialogDescription m_Description = {};
        std::vector<SDL_DialogFileFilter> m_Filters = {};
    };

    class OpenFolderDialogSDL3 final : public OpenFolderDialog
    {
      public:
        OpenFolderDialogSDL3(const OpenFolderDialogDescription &desc);
        virtual ~OpenFolderDialogSDL3();
        FileDialogResult Show() final;

      private:
        OpenFolderDialogDescription m_Description = {};
        std::vector<SDL_DialogFileFilter> m_Filters = {};
    };
} // namespace Nexus