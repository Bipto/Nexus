#pragma once

#include "Nexus-Core/nxpch.hpp"

namespace Nexus
{
	class IWindow;

	enum class DefaultKey
	{
		None,
		Return,
		Escape
	};

	enum class MessageBoxType
	{
		None,
		Information,
		Warning,
		Error
	};

	enum class ButtonLayout
	{
		LeftToRight,
		RightToLeft
	};

	struct MessageBoxButton
	{
		DefaultKey	Key	 = DefaultKey::None;
		int32_t		ID	 = 0;
		std::string Text = "Button";
	};

	struct MessageBoxColour
	{
		uint8_t R = 0;
		uint8_t G = 0;
		uint8_t B = 0;
	};

	struct MessageBoxColourScheme
	{
		MessageBoxColour Background		  = {};
		MessageBoxColour Text			  = {};
		MessageBoxColour ButtonBorder	  = {};
		MessageBoxColour ButtonBackground = {};
		MessageBoxColour ButtonSelected	  = {};
	};

	struct MessageBoxDescription
	{
		std::string							  Title		   = "Title";
		std::string							  Message	   = "Message";
		MessageBoxType						  Type		   = MessageBoxType::Information;
		ButtonLayout						  Layout	   = ButtonLayout::LeftToRight;
		std::vector<MessageBoxButton>		  Buttons	   = {};
		std::optional<MessageBoxColourScheme> ColourScheme = std::nullopt;
		IWindow								 *ParentWindow = nullptr;
	};

	class MessageDialogBox
	{
	  public:
		virtual ~MessageDialogBox()
		{
		}
		virtual const MessageBoxDescription &GetDescription() const = 0;
		virtual int32_t						 Show() const			= 0;
	};
}	 // namespace Nexus