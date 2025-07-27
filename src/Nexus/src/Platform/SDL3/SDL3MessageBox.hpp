#pragma once

#include "Nexus-Core/MessageBox.hpp"

namespace Nexus
{
	class MessageBoxSDL3 final : public MessageDialogBox
	{
	  public:
		MessageBoxSDL3(const MessageBoxDescription &description);
		virtual ~MessageBoxSDL3();
		const MessageBoxDescription &GetDescription() const final;
		int32_t						 Show() const final;

	  private:
		MessageBoxDescription m_Description = {};
	};
}	 // namespace Nexus