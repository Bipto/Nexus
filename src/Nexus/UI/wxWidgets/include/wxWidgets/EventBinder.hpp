#pragma once

#include <functional>
#include <memory>
#include <wx/wx.h>

namespace Nexus::UI
{
    class EventBinder
    {
      public:
        template <typename EventTag>
        EventBinder(wxWindow *win, EventTag evt, wxWindowID id)
            : m_Window(win), m_Id(id), m_Callback(std::make_shared<std::function<void()>>())
        {
            // Store a binder function that knows how to bind this event type
            m_Binder = [win, evt, id](std::shared_ptr<std::function<void()>> cb) {
                win->Bind(
                    evt,
                    [cb](auto &) {
                        if (*cb)
                            (*cb)();
                    },
                    id
                );
            };
        }

        void SetCallback(std::function<void()> func)
        {
            *m_Callback = std::move(func);

            // Re-bind using the stored binder
            m_Binder(m_Callback);
        }

      private:
        wxWindow *m_Window;
        wxWindowID m_Id;

        // Stores the callback safely
        std::shared_ptr<std::function<void()>> m_Callback;

        // Stores a function that knows how to bind the event
        std::function<void(std::shared_ptr<std::function<void()>>)> m_Binder;
    };

} // namespace Nexus::UI
