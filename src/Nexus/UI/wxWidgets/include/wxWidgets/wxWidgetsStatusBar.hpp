#pragma once

#include "UI/StatusBar.hpp"

#include <wx/wx.h>

namespace Nexus::UI
{
    class wxWidgetsStatusBar final : public IStatusBar
    {
      public:
        wxWidgetsStatusBar(wxFrame *frame);
        ~wxWidgetsStatusBar() final = default;
        void SetStatusText(std::string_view text) final;
        void SetHelpText(std::string_view text) final;

      private:
        wxFrame *m_Frame = nullptr;
        wxStatusBar *m_StatusBar = nullptr;
    };
} // namespace Nexus::UI