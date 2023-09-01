#include "Components.hpp"

#include "Core/FileSystem/FileDialogs.hpp"
#include "Core/Runtime.hpp"

namespace Nexus
{

    //-----------------------------------------------------------------------------
    // TRANSFORM COMPONENT
    //-----------------------------------------------------------------------------

    // transform component
    void TransformComponent::RenderUI()
    {
        // ImGui::DragFloat3("Translation", glm::value_ptr(m_Translation), 0.1f, 0.1f, std::numeric_limits<float>::max());
        ImGui::DragFloat3("Translation", glm::value_ptr(m_Translation));
        ImGui::DragFloat3("Rotation", glm::value_ptr(m_Rotation));
        ImGui::DragFloat3("Scale", glm::value_ptr(m_Scale));
    }

    nlohmann::json TransformComponent::Serialize()
    {
        nlohmann::json j;
        j["translation"] =
            {
                {"x", m_Translation.x},
                {"y", m_Translation.y},
                {"z", m_Translation.z}};
        j["rotation"] =
            {
                {"x", m_Rotation.x},
                {"y", m_Rotation.y},
                {"z", m_Rotation.z}};
        j["scale"] =
            {
                {"x", m_Scale.x},
                {"y", m_Scale.y},
                {"z", m_Scale.z}};
        return j;
    }

    void TransformComponent::Deserialize(nlohmann::json json)
    {
        auto translation = json["translation"];
        m_Translation =
            {
                translation["x"],
                translation["y"],
                translation["z"]};

        auto rotation = json["rotation"];
        m_Rotation =
            {
                rotation["x"],
                rotation["y"],
                rotation["z"]};

        auto scale = json["scale"];
        m_Scale =
            {
                scale["x"],
                scale["y"],
                scale["z"]};
    }

    Component *TransformComponent::Clone()
    {
        TransformComponent *component = new TransformComponent();
        component->m_Translation = this->m_Translation;
        component->m_Rotation = this->m_Rotation;
        component->m_Scale = this->m_Scale;
        return component;
    }

    //-----------------------------------------------------------------------------
    // SPRITE RENDERER COMPONENT
    //-----------------------------------------------------------------------------

    void SpriteRendererComponent::RenderUI()
    {
        ImGui::ColorEdit3("Color", glm::value_ptr(m_Color));
        ImGui::Text("Filepath: ");
        ImGui::SameLine();
        ImGui::Text("%s", m_Filepath.c_str());
        if (ImGui::Button("Choose file"))
        {
            std::vector<const char *> filters = {"*.png", "*.jpg"};
            auto path = Nexus::FileDialogs::OpenFile(filters);
            if (path)
                SetFilepath(std::string(path));
        }
    }

    nlohmann::json SpriteRendererComponent::Serialize()
    {
        nlohmann::json j;
        j["color"] =
            {
                {"r", m_Color.r},
                {"g", m_Color.g},
                {"b", m_Color.b}};
        j["filepath"] = m_Filepath;
        return j;
    }

    void SpriteRendererComponent::Deserialize(nlohmann::json json)
    {
        auto color = json["color"];
        m_Color =
            {
                color["r"],
                color["g"],
                color["b"]};
        m_Filepath = json["filepath"];
        LoadTexture();
    }

    Component *SpriteRendererComponent::Clone()
    {
        SpriteRendererComponent *component = new SpriteRendererComponent();
        component->m_Color = this->m_Color;
        component->m_Texture = this->m_Texture;
        return component;
    }

    void SpriteRendererComponent::SetFilepath(const std::string &filepath)
    {
        m_Filepath = filepath;
        LoadTexture();
    }

    void SpriteRendererComponent::LoadTexture()
    {
        m_Texture = nullptr;
        m_Texture = Nexus::GetAssetManager()->GetTexture(m_Filepath);
    }
}