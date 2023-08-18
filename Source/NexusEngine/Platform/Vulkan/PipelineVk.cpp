#include "PipelineVk.hpp"

namespace Nexus::Graphics
{
    PipelineVk::PipelineVk(const PipelineDescription &description)
        : Pipeline(description)
    {
    }

    PipelineVk::~PipelineVk()
    {
    }

    const PipelineDescription &PipelineVk::GetPipelineDescription()
    {
        return m_Description;
    }
}