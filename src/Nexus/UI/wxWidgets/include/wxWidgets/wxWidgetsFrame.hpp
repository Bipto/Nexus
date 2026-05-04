#pragma once

#include <memory>
#include <string>

#include "UI/Frame.hpp"
#include "UI/Menubar.hpp"

#include <wx/wx.h>

namespace Nexus::UI
{
	class wxWidgetsFrame : public IFrame
	{
	  public:
		wxWidgetsFrame(const std::string &title);
		IMenubar *CreateMenubar() final;

	  private:
		wxFrame					 *m_Frame	= nullptr;
		std::unique_ptr<IMenubar> m_Menubar = nullptr;
	};
}	 // namespace Nexus::UI