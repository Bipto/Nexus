#include "SDL3MessageBox.hpp"

#include "SDL3Include.hpp"
#include "SDL3Window.hpp"

namespace Nexus
{
	Uint32 GetMessageBoxButtonFlags(DefaultKey key)
	{
		switch (key)
		{
			case DefaultKey::Escape: return SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
			case DefaultKey::Return: return SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			case DefaultKey::NoKey: return 0;
			default: throw std::runtime_error("Failed to find a valid default key");
		}
	}

	Uint32 GetMessageBoxFlags(const MessageBoxDescription &desc)
	{
		Uint32 flags = 0;

		switch (desc.Type)
		{
			case MessageBoxType::Default:
			{
				break;
			}
			case MessageBoxType::Information:
			{
				flags |= SDL_MESSAGEBOX_INFORMATION;
				break;
			}
			case MessageBoxType::Warning:
			{
				flags |= SDL_MESSAGEBOX_WARNING;
				break;
			}
			case MessageBoxType::Error:
			{
				flags |= SDL_MESSAGEBOX_ERROR;
				break;
			}
			default: throw std::runtime_error("Failed to find a valid message box type");
		}

		switch (desc.Layout)
		{
			case ButtonLayout::LeftToRight:
			{
				flags |= SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
				break;
			}
			case ButtonLayout::RightToLeft:
			{
				flags |= SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT;
				break;
			}
			default: throw std::runtime_error("Failed to find a valid layout");
		}

		return flags;
	}

	void GetSDLMessageBoxColourScheme(const MessageBoxColourScheme &colourScheme, SDL_MessageBoxColorScheme &sdlScheme)
	{
		sdlScheme.colors[0] = {colourScheme.Background.R, colourScheme.Background.G, colourScheme.Background.B};
		sdlScheme.colors[1] = {colourScheme.Text.R, colourScheme.Text.G, colourScheme.Text.B};
		sdlScheme.colors[2] = {colourScheme.ButtonBorder.R, colourScheme.ButtonBorder.G, colourScheme.ButtonBorder.B};
		sdlScheme.colors[3] = {colourScheme.ButtonBackground.R, colourScheme.ButtonBackground.G, colourScheme.ButtonBackground.B};
		sdlScheme.colors[4] = {colourScheme.ButtonSelected.R, colourScheme.ButtonSelected.G, colourScheme.ButtonSelected.B};
	}

	MessageBoxSDL3::MessageBoxSDL3(const MessageBoxDescription &description) : m_Description(description)
	{
	}

	MessageBoxSDL3::~MessageBoxSDL3()
	{
	}

	const MessageBoxDescription &MessageBoxSDL3::GetDescription() const
	{
		return m_Description;
	}

	int32_t MessageBoxSDL3::Show() const
	{
		SDL_MessageBoxColorScheme sdlColourScheme = {};

		std::vector<SDL_MessageBoxButtonData> buttons = {};
		for (const auto &button : m_Description.Buttons)
		{
			SDL_MessageBoxButtonData &b = buttons.emplace_back();
			b.flags						= GetMessageBoxButtonFlags(button.Key);
			b.buttonID					= button.ID;
			b.text						= button.Text.c_str();
		}

		SDL_MessageBoxData messageBoxData = {};
		messageBoxData.title			  = m_Description.Title.c_str();
		messageBoxData.message			  = m_Description.Message.c_str();
		messageBoxData.flags			  = GetMessageBoxFlags(m_Description);
		messageBoxData.buttons			  = buttons.data();
		messageBoxData.numbuttons		  = buttons.size();

		if (m_Description.ParentWindow)
		{
			SDL3Window *window	  = (SDL3Window *)m_Description.ParentWindow;
			messageBoxData.window = window->GetSDLWindowHandle();
		}

		if (m_Description.ColourScheme)
		{
			MessageBoxColourScheme colourScheme = m_Description.ColourScheme.value();
			GetSDLMessageBoxColourScheme(colourScheme, sdlColourScheme);
			messageBoxData.colorScheme = &sdlColourScheme;
		}

		int32_t buttonId = {};
		SDL_ShowMessageBox(&messageBoxData, &buttonId);
		return buttonId;
	}
}	 // namespace Nexus