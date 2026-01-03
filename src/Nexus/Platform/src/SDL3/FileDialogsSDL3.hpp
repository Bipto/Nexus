#pragma once

#include "Platform/FileSystem/FileDialogs.hpp"
#include "Platform/IWindow.hpp"
#include "SDL3Include.hpp"

namespace Nexus
{
	class OpenFileDialogSDL3 final : public OpenFileDialog
	{
	  public:
		OpenFileDialogSDL3(IWindow *window, const std::vector<FileDialogFilter> &filters, std::string_view defaultLocation, bool allowMany);
		virtual ~OpenFileDialogSDL3();
		FileDialogResult Show() final;

	  private:
		IWindow							 *m_Window			= nullptr;
		std::vector<SDL_DialogFileFilter> m_Filters			= {};
		std::string						  m_DefaultLocation = {};
		bool							  m_AllowMany		= false;
	};

	class SaveFileDialogSDL3 final : public SaveFileDialog
	{
	  public:
		SaveFileDialogSDL3(IWindow *window, const std::vector<FileDialogFilter> &filters, std::string_view defaultLocation);
		virtual ~SaveFileDialogSDL3();
		FileDialogResult Show() final;

	  private:
		IWindow							 *m_Window			= nullptr;
		std::vector<SDL_DialogFileFilter> m_Filters			= {};
		std::string						  m_DefaultLocation = {};
	};

	class OpenFolderDialogSDL3 final : public OpenFolderDialog
	{
	  public:
		OpenFolderDialogSDL3(IWindow *window, std::string_view defaultLocation, bool allowMany);
		virtual ~OpenFolderDialogSDL3();
		FileDialogResult Show() final;

	  private:
		IWindow	   *m_Window		  = nullptr;
		std::string m_DefaultLocation = {};
		bool		m_AllowMany		  = false;
	};
}	 // namespace Nexus