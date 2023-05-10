#include "Panel.h"

class ShaderViewerPanel : public Panel
{
    public:
        ShaderViewerPanel(Nexus::Ref<Nexus::Shader> shader)
             : m_Shader(shader)
             {}

        virtual void OnRender() override
        {
            ImGui::Begin("Shader Viewer", &m_Enabled);
            ImGui::Text("Vertex Shader");
            ImGui::Text(m_Shader->GetVertexShaderSource().c_str());
            ImGui::Separator();
            ImGui::Text("Fragment Shader");
            ImGui::Text(m_Shader->GetFragmentShaderSource().c_str());
            ImGui::Separator();        
            ImGui::End();
        }
        
    private:
        Nexus::Ref<Nexus::Shader> m_Shader;
};