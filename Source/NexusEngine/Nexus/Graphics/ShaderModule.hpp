#pragma once

#include "ResourceSet.hpp"

#include <string>

namespace Nexus::Graphics
{
    enum class ShaderStage
    {
        None = 0,
        Compute,
        Fragment,
        Geometry,
        TesselationControl,
        TesselationEvaluation,
        Vertex
    };

    struct ShaderModuleSpecification
    {
        std::string Name = "ShaderModule";
        std::string Source;
        ShaderStage Stage = ShaderStage::None;
        std::vector<uint32_t> SpirvBinary;
    };

    class ShaderModule
    {
    public:
        ShaderModule(const ShaderModuleSpecification &shaderModuleSpec, const ResourceSetSpecification &resourceSpec)
            : m_ModuleSpecification(shaderModuleSpec), m_ResourceSetSpecification(resourceSpec) {}

        virtual ~ShaderModule() = default;

        ShaderStage GetShaderStage() const { return m_ModuleSpecification.Stage; }
        const ShaderModuleSpecification &GetModuleSpecification() const { return m_ModuleSpecification; }
        const ResourceSetSpecification &GetResourceSetSpecification() const { return m_ResourceSetSpecification; }

    protected:
        ShaderModuleSpecification m_ModuleSpecification;
        ResourceSetSpecification m_ResourceSetSpecification;
    };
}