#pragma once

#include <string>
#include <limits>
#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Nexus/Graphics/Texture.hpp"
#include "Nexus/Types.hpp"

#include "imgui.h"
#include "nlohmann/json.hpp"

namespace Nexus
{
    class Component
    {
    public:
        virtual ~Component() {}
        virtual const char *GetName() = 0;
        virtual void RenderUI() = 0;
        virtual nlohmann::json Serialize() = 0;
        virtual void Deserialize(nlohmann::json json) = 0;
        virtual Component *Clone() = 0;
    };

    class TransformComponent : public Component
    {
    public:
        virtual const char *GetName() override { return "Transform"; }
        virtual void RenderUI() override;
        virtual nlohmann::json Serialize() override;
        virtual void Deserialize(nlohmann::json json) override;
        virtual Component *Clone() override;

        const glm::vec3 GetTranslation() { return m_Translation; }
        const glm::vec3 GetRotation() { return m_Rotation; }
        const glm::vec3 GetScale() { return m_Scale; }
        void SetTranslation(glm::vec3 translation) { m_Translation = translation; }
        void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; }
        void SetScale(glm::vec3 scale) { m_Scale = scale; }

    private:
        glm::vec3 m_Translation = glm::vec3(0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);
        glm::vec3 m_Scale = glm::vec3(1.0f);
    };

    class SpriteRendererComponent : public Component
    {
    public:
        virtual const char *GetName() override { return "SpriteRenderer"; }
        virtual void RenderUI() override;
        virtual nlohmann::json Serialize() override;
        virtual void Deserialize(nlohmann::json json) override;
        virtual Component *Clone() override;

        const glm::vec3 &GetColor() const { return m_Color; }
        glm::vec3 &GetColor() { return m_Color; }
        void SetColor(glm::vec3 color) { m_Color = color; }

        const std::string &GetFilepath() { return m_Filepath; }
        void SetFilepath(const std::string &filepath);

        Graphics::Texture *GetTexture() { return m_Texture; }

        void LoadTexture();

    private:
        glm::vec3 m_Color{1.0f, 1.0f, 1.0f};
        Graphics::Texture *m_Texture;
        std::string m_Filepath;
    };
}