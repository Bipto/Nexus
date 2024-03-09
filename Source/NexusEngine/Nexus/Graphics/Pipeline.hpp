#pragma once

#include "Shader.hpp"
#include "Viewport.hpp"
#include "Scissor.hpp"

#include "ResourceSet.hpp"
#include "RenderTarget.hpp"

#include "Structures.hpp"

namespace Nexus::Graphics
{
    /// @brief A struct representing how all of the settings required to create a pipeline
    struct PipelineDescription
    {
        /// @brief How the pipeline should handle depth and stencil testing
        DepthStencilDescription DepthStencilDescription;

        /// @brief How the pipeline should handle rasterization
        RasterizerStateDescription RasterizerStateDescription;

        /// @brief How the pipeline should handle blending
        BlendStateDescription BlendStateDescription;

        /// @brief How the pipeline should handle the vertex buffer data
        Topology PrimitiveTopology = Topology::TriangleList;

        /// @brief A pointer to a shader that should be used for rendering
        Shader *Shader;

        /// @brief A resource set specification describing how resources are allocated in the pipeline
        ResourceSetSpecification ResourceSetSpecification;

        /// @brief The target to render to with the pipeline
        RenderTarget Target;

        /// @brief The layout of data within bound vertex buffers
        std::vector<VertexBufferLayout> Layouts;
    };

    /// @brief A pure virtual class representing an API specific pipeline
    class Pipeline
    {
    public:
        /// @brief A constructor that takes in a PipelineDescription object to use for creation
        Pipeline(const PipelineDescription &description)
        {
            m_Description = description;
        }

        /// @brief An empty pipeline cannot be created
        Pipeline() = delete;

        /// @brief Virtual destructor allowing API specific resources to be destroyed
        virtual ~Pipeline(){};

        /// @brief A pure virtual method returning a const reference to a pipeline description
        /// @return A const reference to a pipelinedescription
        virtual const PipelineDescription &GetPipelineDescription() const = 0;

        /// @brief A method that returns a reference counted pointer to a shader
        /// @return A pointer to the shader used by the pipeline
        virtual Shader *GetShader() const { return m_Description.Shader; }

    protected:
        /// @brief The pipeline description used to create the pipeline
        PipelineDescription m_Description;

    private:
        /// @brief This allows the GraphicsDevice to access all data stored within a pipeline
        friend class GraphicsDevice;
    };
}