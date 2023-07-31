#include "Panel.hpp"

class ShaderViewerPanel : public Panel
{
public:
    ShaderViewerPanel(Nexus::Ref<Nexus::Graphics::Shader> shader)
        : m_Shader(shader)
    {
    }

    virtual void OnRender() override
    {
        ImGui::Begin("Shader Viewer", &m_Enabled);
        ImGui::Text("Vertex Shader");
        ImGui::Text("%s", m_Shader->GetVertexShaderSource().c_str());
        ImGui::Separator();
        ImGui::Text("Fragment Shader");
        ImGui::Text("%s", m_Shader->GetFragmentShaderSource().c_str());
        ImGui::Separator();
        ImGui::End();
    }

private:
    Nexus::Ref<Nexus::Graphics::Shader> m_Shader;
};