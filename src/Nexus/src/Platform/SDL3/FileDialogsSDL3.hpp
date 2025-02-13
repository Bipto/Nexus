#pragma once

#include "Nexus-Core/FileSystem/FileDialogs.hpp"
#include "Nexus-Core/IWindow.hpp"
#include "SDL3Include.hpp"

namespace Nexus
{
	class OpenFileDialogSDL3 : public OpenFileDialog
	{
	  public:
		OpenFileDialogSDL3(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation, bool allowMany);
		virtual ~OpenFileDialogSDL3();
		FileDialogResult Show() final;

	  private:
		IWindow							 *m_Window			= nullptr;
		std::vector<SDL_DialogFileFilter> m_Filters			= {};
		const char						 *m_DefaultLocation = nullptr;
		bool							  m_AllowMany		= false;
	};

	class SaveFileDialogSDL3 : public SaveFileDialog
	{
	  public:
		SaveFileDialogSDL3(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation);
		virtual ~SaveFileDialogSDL3();
		FileDialogResult Show() final;

	  private:
		IWindow							 *m_Window			= nullptr;
		std::vector<SDL_DialogFileFilter> m_Filters			= {};
		const char						 *m_DefaultLocation = nullptr;
	};

	class OpenFolderDialogSDL3 : public OpenFolderDialog
	{
	  public:
		OpenFolderDialogSDL3(IWindow *window, const char *defaultLocation, bool allowMany);
		virtual ~OpenFolderDialogSDL3();
		FileDialogResult Show() final;

	  private:
		IWindow	   *m_Window		  = nullptr;
		const char *m_DefaultLocation = nullptr;
		bool		m_AllowMany		  = false;
	};
}	 // namespace Nexus