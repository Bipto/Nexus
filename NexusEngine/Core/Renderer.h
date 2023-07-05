#pragma once
#include "Core/Graphics/GraphicsDevice.h"
#include "glm/glm.hpp"

namespace Nexus
{
    /// @brief A class representing a renderer
    class Renderer
    {
    public:
        /// @brief A method that starts a scene render
        /// @param vp The view projection matrix to use to render
        /// @param clearColor A colour to use to clear the screen when rendering
        void Begin(const glm::mat4 &vp, const glm::vec4 &clearColor);

        /// @brief A method that ends a scene render
        void End();

    public:
        /// @brief A static method that creates a new renderer
        /// @param device A reference counted pointer to a graphics device
        /// @return A pointer to the new renderer
        static Renderer *Create(Ref<GraphicsDevice> device);

    private:
        /// @brief A private constructor that takes in a graphics device
        /// @param device A reference counted pointer to the graphics device
        Renderer(Ref<GraphicsDevice> device);

    private:
        /// @brief A reference counted pointer to a graphics device
        Ref<GraphicsDevice> m_GraphicsDevice;

        /// @brief A view projection matrix
        glm::mat4 m_VP;
    };
}