#include <mutex>

#include "FileDialogsSDL3.hpp"
#include "Platform/Platform.hpp"
#include "SDL3Window.hpp"

namespace Nexus
{

	/// @brief A struct that contains all relevant data to show a file dialog with SDL3
	struct SDLDialogData
	{
		/// @brief A boolean indicating whether the dialog has finished being used by the user
		bool dialogFinished = false;

		/// @brief A mutex to synchronise the main thread and the (potentially) different thread that the callback is called on
		std::mutex dialogMutex;

		/// @brief A returned struct containing data about how the user used the dialog
		FileDialogResult dialogResult;
	};

	/// @brief A function that is called when the user accepts or cancels a file dialog
	/// @param userdata A void pointer that contains a pointer to a SDLDialogData struct
	/// @param filelist A pointer to an array of pointers to strings containing the files selected by the user
	/// @param filter An integer containing which filter the user used when selecting files
	static void sdl_file_selected_callback(void *userdata, const char *const *filelist, int filter)
	{
		// retrieve the data from the callback
		SDLDialogData *data = (SDLDialogData *)userdata;

		// lock the data so it cannot be accessed by any other threads
		std::lock_guard<std::mutex> guard(data->dialogMutex);

		// assign the filter that was used by the user
		data->dialogResult.SelectedFilter = filter;

		// if the user selected a file, we need to do futher processing
		if (*filelist)
		{
			// iterate through each file in the list
			while (*filelist)
			{
				// check that the filepath is a valid string pointer
				if (filelist != nullptr)
				{
					// retrieve the filepath from the data and insert into the vector of paths to return
					std::string filepath = std::string(*filelist);
					data->dialogResult.FilePaths.push_back(filepath);
				}

				// increment the array pointer
				filelist++;
			}
		}

		// tell the application that the dialog has been finished so that normal application flow can continue
		data->dialogFinished = true;
	}

	OpenFileDialogSDL3::OpenFileDialogSDL3(const OpenFileDialogDescription &desc) : m_Description(desc)
	{
		// convert the Nexus::FileDialogFilter structs into SDL_DialogFileFilters
		m_Filters.reserve(m_Description.Filters.size());
		for (const auto &[name, pattern] : m_Description.Filters) { m_Filters.emplace_back(name, pattern); }
	}

	static FileDialogResult ShowSDL3DialogBox(IWindow							*window,
											  std::vector<SDL_DialogFileFilter> &filters,
											  std::optional<std::string>		 defaultLocation,
											  std::optional<bool>				 allowMany,
											  const std::string					&title,
											  const std::string					&acceptString,
											  const std::string					&cancelString,
											  SDL_FileDialogType				 dialogType)
	{
		// create the properties to use when displaying the dialog
		SDL_PropertiesID properties = SDL_CreateProperties();

		// set up the filters for the dialog
		SDL_SetPointerProperty(properties, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, filters.data());
		SDL_SetNumberProperty(properties, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, filters.size());

		if (window)
		{
			// set up a pointer to the window that owns the dialog
			SDL3Window *wrapper = reinterpret_cast<SDL3Window *>(window);
			SDL_SetPointerProperty(properties, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, wrapper->GetSDLWindowHandle());
		}

		if (defaultLocation)
		{
			// set up a default path for the dialog
			SDL_SetStringProperty(properties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, defaultLocation.value().c_str());
		}

		if (allowMany)
		{
			// set up whether the dialog can accept multiple files
			SDL_SetBooleanProperty(properties, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, allowMany.value());
		}

		// set up title and labels
		SDL_SetStringProperty(properties, SDL_PROP_FILE_DIALOG_TITLE_STRING, title.c_str());
		SDL_SetStringProperty(properties, SDL_PROP_FILE_DIALOG_ACCEPT_STRING, acceptString.c_str());
		SDL_SetStringProperty(properties, SDL_PROP_FILE_DIALOG_CANCEL_STRING, cancelString.c_str());

		// data to be written to in callback
		SDLDialogData data = {};

		// display the dialog using the callback
		SDL_ShowFileDialogWithProperties(dialogType, sdl_file_selected_callback, &data, properties);

		// wait and keep updating events until the user selects a file
		while (!data.dialogFinished) { Nexus::Platform::PollEvents(); }

		// return the completed dialog result
		return data.dialogResult;
	}

	OpenFileDialogSDL3::~OpenFileDialogSDL3()
	{
	}

	FileDialogResult OpenFileDialogSDL3::Show()
	{
		return ShowSDL3DialogBox(m_Description.WindowHandle,
								 m_Filters,
								 m_Description.DefaultLocation,
								 m_Description.AllowMany,
								 m_Description.TitleString,
								 m_Description.AcceptString,
								 m_Description.CancelString,
								 SDL_FILEDIALOG_OPENFILE);
	}

	SaveFileDialogSDL3::SaveFileDialogSDL3(const SaveFileDialogDescription &desc) : m_Description(desc)
	{
		// convert the Nexus::FileDialogFilter structs into SDL_DialogFileFilters
		m_Filters.reserve(m_Description.Filters.size());
		for (const auto &[name, pattern] : m_Description.Filters) { m_Filters.emplace_back(name, pattern); }
	}

	SaveFileDialogSDL3::~SaveFileDialogSDL3()
	{
	}

	FileDialogResult SaveFileDialogSDL3::Show()
	{
		return ShowSDL3DialogBox(m_Description.WindowHandle,
								 m_Filters,
								 m_Description.DefaultLocation,
								 std::nullopt,
								 m_Description.TitleString,
								 m_Description.AcceptString,
								 m_Description.CancelString,
								 SDL_FILEDIALOG_OPENFILE);
	}

	OpenFolderDialogSDL3::OpenFolderDialogSDL3(const OpenFolderDialogDescription &desc) : m_Description(desc)
	{
	}

	OpenFolderDialogSDL3::~OpenFolderDialogSDL3()
	{
	}

	FileDialogResult OpenFolderDialogSDL3::Show()
	{
		return ShowSDL3DialogBox(m_Description.WindowHandle,
								 m_Filters,
								 m_Description.DefaultLocation,
								 m_Description.AllowMany,
								 m_Description.TitleString,
								 m_Description.AcceptString,
								 m_Description.CancelString,
								 SDL_FILEDIALOG_OPENFOLDER);
	}

}	 // namespace Nexus
