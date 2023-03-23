#include "Components.h"

namespace Nexus
{
    //transform component
    void TransformComponent::RenderUI()
    {
        ImGui::DragFloat3("Translation", glm::value_ptr(m_Translation), 0.1f, 0.1f, std::numeric_limits<float>::max());
        ImGui::DragFloat3("Rotation", glm::value_ptr(m_Rotation));
        ImGui::DragFloat3("Scale", glm::value_ptr(m_Scale));
    }

    nlohmann::json TransformComponent::Serialize()
    {
        nlohmann::json j;
        j["translation"] = 
        {
            { "x", m_Translation.x },
            { "y", m_Translation.y },
            { "z", m_Translation.z}
        };
        j["rotation"] = 
        {
            { "x", m_Rotation.x },
            { "y", m_Rotation.y },
            { "z", m_Rotation.z }
        };
        j["scale"] = 
        {
            { "x", m_Scale.x },
            { "y", m_Scale.y },
            { "z", m_Scale.z }
        };
        return j;
    }

    void TransformComponent::Deserialize(nlohmann::json json)
    {
        auto translation = json["translation"];
        m_Translation = 
        {
            translation["x"],
            translation["y"],
            translation["z"]
        };

        auto rotation = json["rotation"];
        m_Rotation = 
        {
            rotation["x"],
            rotation["y"],
            rotation["z"]
        };

        auto scale = json["scale"];
        m_Scale = 
        {
            scale["x"],
            scale["y"],
            scale["z"]
        };
    }
}