#include "Panel.h"

#include "Core/Logging/Log.h"

class LogPanel : public Panel
{
    public:
        virtual void OnRender() override
        {
            auto logs = Nexus::GetCoreLogger()->GetLogs();

            ImGui::Begin("Log");

            for (auto& log : logs)
            {
                ImGui::Text(log.Message.c_str());
                ImGui::Separator();
            }

            ImGui::End();
        }
};