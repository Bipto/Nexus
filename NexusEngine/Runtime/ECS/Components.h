#pragma once

#include <string>
#include <limits>
#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Core/Graphics/Texture.h"
#include "Core/Memory.h"

#include "imgui.h"
#include "nlohmann/json.hpp"


namespace Nexus
{
    class Component
    {
        public:
            virtual const char* GetName() = 0;
            virtual void RenderUI() = 0;
            virtual nlohmann::json Serialize() = 0;
            virtual void Deserialize(nlohmann::json json) = 0;
            virtual Component* Clone() = 0;
    };

    class TransformComponent : public Component
    {
        public:
            virtual const char* GetName() override { return "Transform"; }
            virtual void RenderUI() override;
            virtual nlohmann::json Serialize() override;
            virtual void Deserialize(nlohmann::json json) override;
            virtual Component* Clone() override;


            const glm::vec3 GetTranslation() { return m_Translation; }
            const glm::vec3 GetRotation() { return m_Rotation; }
            const glm::vec3 GetScale() { return m_Scale; }
            void SetTranslation(glm::vec3 translation) { m_Translation = translation; }
            void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; }
            void SetScale(glm::vec3 scale) { m_Scale = scale; }
        private:
            glm::vec3 m_Translation;
            glm::vec3 m_Rotation;
            glm::vec3 m_Scale;
    };

    class SpriteRendererComponent : public Component
    {
        public:
            virtual const char* GetName() override { return "Transform"; }
            virtual void RenderUI() override;
            virtual nlohmann::json Serialize() override;
            virtual void Deserialize(nlohmann::json json) override;
            virtual Component* Clone() override;

            const glm::vec4 GetColor() { return m_Color; }
            void SetColor(glm::vec4 color) { m_Color = color; }
        private:
            glm::vec4 m_Color;
            Ref<Texture> m_Texture;
    };
}