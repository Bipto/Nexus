#pragma once

#include "Core/Graphics/Pipeline.hpp"

namespace Nexus::Graphics
{
    class PipelineVk : public Pipeline
    {
    public:
        PipelineVk(const PipelineDescription &description);
        ~PipelineVk();
        virtual const PipelineDescription &GetPipelineDescription() const override;
    };
}