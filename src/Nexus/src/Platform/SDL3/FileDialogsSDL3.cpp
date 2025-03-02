#include "FileDialogsSDL3.hpp"

#include "Nexus-Core/Platform.hpp"

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
	void sdl_file_selected_callback(void *userdata, const char *const *filelist, int filter)
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

	OpenFileDialogSDL3::OpenFileDialogSDL3(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation, bool allowMany)
		: m_Window(window),
		  m_DefaultLocation(defaultLocation),
		  m_AllowMany(allowMany)
	{
		// convert the Nexus::FileDialogFilter structs into SDL_DialogFileFilters
		m_Filters.reserve(filters.size());
		for (const auto &[name, pattern] : filters) { m_Filters.emplace_back(name, pattern); }
	}

	OpenFileDialogSDL3::~OpenFileDialogSDL3()
	{
	}

	FileDialogResult OpenFileDialogSDL3::Show()
	{
		// retrieve the interface implementation of the window
		SDL3Window *wrapper = (SDL3Window *)m_Window;

		// data to be written to in callback
		SDLDialogData data = {};

		// create the properties to use when displaying the dialog
		SDL_PropertiesID properties = SDL_CreateProperties();

		// set up the filters for the dialog
		SDL_SetPointerProperty(properties, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, m_Filters.data());
		SDL_SetNumberProperty(properties, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, m_Filters.size());

		// set up a pointer to the window that owns the dialog
		SDL_SetPointerProperty(properties, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, wrapper->GetSDLWindowHandle());

		// set up a default path for the dialog
		SDL_SetStringProperty(properties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, m_DefaultLocation);

		// set up whether the dialog can accept multiple files
		SDL_SetBooleanProperty(properties, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, m_AllowMany);

		// display the dialog using the callback
		SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_OPENFILE, sdl_file_selected_callback, &data, properties);

		// wait and keep updating events until the user selects a file
		while (!data.dialogFinished) { Nexus::Platform::PollEvents(); }

		// return the completed dialog result
		return data.dialogResult;
	}

	SaveFileDialogSDL3::SaveFileDialogSDL3(IWindow *window, const std::vector<FileDialogFilter> &filters, const char *defaultLocation)
		: m_Window(window),
		  m_DefaultLocation(defaultLocation)
	{
		// convert the Nexus::FileDialogFilter structs into SDL_DialogFileFilters
		m_Filters.reserve(filters.size());
		for (const auto &[name, pattern] : filters) { m_Filters.emplace_back(name, pattern); }
	}

	SaveFileDialogSDL3::~SaveFileDialogSDL3()
	{
	}

	FileDialogResult SaveFileDialogSDL3::Show()
	{
		// retrieve the interface implementation of the window
		SDL3Window *wrapper = (SDL3Window *)m_Window;

		// data to be written to in callback
		SDLDialogData data = {};

		// create the properties to use when displaying the dialog
		SDL_PropertiesID properties = SDL_CreateProperties();

		// set up the filters for the dialog
		SDL_SetPointerProperty(properties, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, m_Filters.data());
		SDL_SetNumberProperty(properties, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, m_Filters.size());

		// set up a pointer to the window that owns the dialog
		SDL_SetPointerProperty(properties, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, wrapper->GetSDLWindowHandle());

		// set up a default path for the dialog
		SDL_SetStringProperty(properties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, m_DefaultLocation);

		// display the dialog using the callback
		SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_SAVEFILE, sdl_file_selected_callback, &data, properties);

		// wait and keep updating events until the user selects a file
		while (!data.dialogFinished) { Nexus::Platform::PollEvents(); }

		// return the completed dialog result
		return data.dialogResult;
	}

	OpenFolderDialogSDL3::OpenFolderDialogSDL3(IWindow *window, const char *defaultLocation, bool allowMany)
		: m_Window(window),
		  m_DefaultLocation(defaultLocation),
		  m_AllowMany(allowMany)
	{
	}

	OpenFolderDialogSDL3::~OpenFolderDialogSDL3()
	{
	}

	FileDialogResult OpenFolderDialogSDL3::Show()
	{
		// retrieve the interface implementation of the window
		SDL3Window *wrapper = (SDL3Window *)m_Window;

		// data to be written to in callback
		SDLDialogData data = {};

		// create the properties to use when displaying the dialog
		SDL_PropertiesID properties = SDL_CreateProperties();

		// set up a pointer to the window that owns the dialog
		SDL_SetPointerProperty(properties, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, wrapper->GetSDLWindowHandle());

		// set up a default path for the dialog
		SDL_SetStringProperty(properties, SDL_PROP_FILE_DIALOG_LOCATION_STRING, m_DefaultLocation);

		// set up whether the dialog can accept multiple files
		SDL_SetBooleanProperty(properties, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, m_AllowMany);

		// display the dialog using the callback
		SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_OPENFOLDER, sdl_file_selected_callback, &data, properties);

		// wait and keep updating events until the user selects a file
		while (!data.dialogFinished) { Nexus::Platform::PollEvents(); }

		// return the completed dialog result
		return data.dialogResult;
	}

}	 // namespace Nexus
