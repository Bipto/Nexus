#include "Panel.hpp"

#include "Core/Logging/Log.hpp"

class LogPanel : public Panel
{
public:
    virtual void OnRender() override
    {
        auto logs = Nexus::GetCoreLogger()->GetLogs();

        ImGui::Begin("Log");
        for (auto &log : logs)
        {
            ImGui::Text("%s", log.Message.c_str());
            ImGui::Separator();
        }
        ImGui::End();
    }
};