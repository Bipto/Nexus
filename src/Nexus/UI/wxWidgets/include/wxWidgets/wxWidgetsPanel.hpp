#pragma once

#include <memory>
#include <string>
#include <vector>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "UI/Control.hpp"
#include "UI/Menubar.hpp"
#include "UI/Panel.hpp"

#include "wxWidgets/wxWidgetsMenubar.hpp"

namespace Nexus::UI
{
    class wxWidgetsPanel final : public IPanel
    {
      public:
        wxWidgetsPanel(wxFrame *frame);
        ~wxWidgetsPanel() final = default;
        IButton *CreateButton(std::string_view text) final;

        wxPanel *GetPanel();

      private:
        wxFrame *m_Frame = nullptr;
        wxPanel *m_Panel = nullptr;

        std::unique_ptr<wxWidgetsMenubar> m_Menubar = nullptr;
    };
} // namespace Nexus::UI