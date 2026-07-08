#include "RHI/ResourceSet.hpp"
#include "RHI/Pipeline.hpp"

#include <cstring>

namespace Nexus::Graphics
{
    static void CreateInitialStorageForDescriptors(
        const ResourceSetDescription &resourceDesc, ResourceSetDescriptors &boundResources,
        ResourceSetDescriptors &queuedResources
    )
    {
        // iterate through all descriptors
        for (const ResourceDescriptor &descriptor : resourceDesc.Descriptors)
        {
            switch (descriptor.Type)
            {
            // push constants are not recorded within the descriptors
            case ResourceDescriptorType::PushConstants:
                continue;
            case ResourceDescriptorType::UniformBuffer:
            {
                boundResources.UniformBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.UniformBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::DynamicUniformBuffer:
            {
                boundResources.DynamicUniformBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.DynamicUniformBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::InlineUniformBlock:
            {
                boundResources.InlineUniformBlocks[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.InlineUniformBlocks[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::StorageBuffer:
            {
                boundResources.StorageBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.StorageBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::DynamicStorageBuffer:
            {
                boundResources.DynamicStorageBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.DynamicStorageBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::StorageImage:
            {
                boundResources.StorageImages[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.StorageImages[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::CombinedImageSampler:
            {
                boundResources.CombinedImageSamplers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.CombinedImageSamplers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::SampledImage:
            {
                boundResources.SampledImages[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.SampledImages[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::Sampler:
            {
                boundResources.Samplers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.Samplers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::AccelerationStructure:
            {
                boundResources.AccelerationStructures[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.AccelerationStructures[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::UniformTexelBuffer:
            {
                boundResources.UniformTexelBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.UniformTexelBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            case ResourceDescriptorType::StorageTexelBuffer:
            {
                boundResources.StorageTexelBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                queuedResources.StorageTexelBuffers[descriptor.Name].resize(descriptor.CountOrSizeInBytes);
                break;
            }
            default:
                throw std::runtime_error("Failed to find a valid descriptor type");
            }
        }
    }

    IResourceSet::IResourceSet(PipelineHandle pipeline) : m_Pipeline(pipeline)
    {
        m_ShaderResources = pipeline->GetRequiredShaderResources();
        const ResourceSetDescription &resourceSetDesc = pipeline->GetResourceSetDescription();
        CreateInitialStorageForDescriptors(resourceSetDesc, m_BoundResources, m_QueuedResources);
    }

    IResourceSet::~IResourceSet()
    {
    }

    void IResourceSet::WriteUniformBuffer(const UniformBufferView &uniformBuffers, const std::string &name)
    {
        WriteUniformBuffers(&uniformBuffers, name, 0, 1);
    }

    void IResourceSet::WriteDynamicUniformBuffer(const UniformBufferView &uniformBuffers, const std::string &name)
    {
        WriteDynamicUniformBuffers(&uniformBuffers, name, 0, 1);
    }

    void IResourceSet::WriteInlineUniformBlock(const void *data, size_t sizeInBytes, const std::string &name)
    {
        auto &inlineBlock = m_QueuedResources.InlineUniformBlocks[name];
        inlineBlock.resize(sizeInBytes);
        memcpy(inlineBlock.data(), data, sizeInBytes);
    }

    void IResourceSet::WriteStorageBuffer(const StorageBufferView &views, const std::string &name)
    {
        WriteStorageBuffers(&views, name, 0, 1);
    }

    void IResourceSet::WriteDynamicStorageBuffer(const StorageBufferView &storageBuffers, const std::string &name)
    {
        WriteDynamicStorageBuffers(&storageBuffers, name, 0, 1);
    }

    void IResourceSet::WriteStorageImage(const StorageImageView &views, const std::string &name)
    {
        WriteStorageImages(&views, name, 0, 1);
    }

    void IResourceSet::WriteCombinedImageSampler(
        const CombinedImageSampler &combinedImageSamplers, const std::string &name
    )
    {
        WriteCombinedImageSamplers(&combinedImageSamplers, name, 0, 1);
    }

    void IResourceSet::WriteSampledImage(TextureViewHandle textureViews, const std::string &name)
    {
        WriteSampledImages(&textureViews, name, 0, 1);
    }

    void IResourceSet::WriteSampler(SamplerHandle sampler, const std::string &name)
    {
        WriteSamplers(&sampler, name, 0, 1);
    }

    void IResourceSet::WriteAccelerationStructure(
        AccelerationStructureHandle accelerationStructure, const std::string &name
    )
    {
        WriteAccelerationStructures(&accelerationStructure, name, 0, 1);
    }

    void IResourceSet::WriteUniformTexelBuffer(TexelBufferHandle texelBuffers, const std::string &name)
    {
        WriteUniformTexelBuffers(&texelBuffers, name, 0, 1);
    }

    void IResourceSet::WriteStorageTexelBuffer(TexelBufferHandle texelBuffers, const std::string &name)
    {
        WriteStorageTexelBuffers(&texelBuffers, name, 0, 1);
    }

    void IResourceSet::WriteUniformBuffers(
        const UniformBufferView *uniformBuffers, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.UniformBuffers[name][index] = *uniformBuffers;
            uniformBuffers++;
        }
    }

    void IResourceSet::WriteDynamicUniformBuffers(
        const UniformBufferView *uniformBuffers, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.DynamicUniformBuffers[name][index] = *uniformBuffers;
            uniformBuffers++;
        }
    }

    void IResourceSet::WriteStorageBuffers(
        const StorageBufferView *views, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.StorageBuffers[name][index] = *views;
            views++;
        }
    }

    void IResourceSet::WriteDynamicStorageBuffers(
        const StorageBufferView *storageBuffers, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.DynamicStorageBuffers[name][index] = *storageBuffers;
            storageBuffers++;
        }
    }

    void IResourceSet::WriteStorageImages(
        const StorageImageView *views, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.StorageImages[name][index] = *views;
            views++;
        }
    }

    void IResourceSet::WriteCombinedImageSamplers(
        const CombinedImageSampler *combinedImageSamplers, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.CombinedImageSamplers[name][index] = *combinedImageSamplers;
            combinedImageSamplers++;
        }
    }

    void IResourceSet::WriteSampledImages(
        TextureViewHandle *textureViews, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.SampledImages[name][index] = *textureViews;
            textureViews++;
        }
    }

    void IResourceSet::WriteSamplers(
        SamplerHandle *samplers, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.Samplers[name][index] = *samplers;
            samplers++;
        }
    }

    void IResourceSet::WriteAccelerationStructures(
        AccelerationStructureHandle *accelerationStructures, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.AccelerationStructures[name][index] = *accelerationStructures;
            accelerationStructures++;
        }
    }

    void IResourceSet::WriteUniformTexelBuffers(
        TexelBufferHandle *texelBuffers, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.UniformTexelBuffers[name][index] = *texelBuffers;
            texelBuffers++;
        }
    }

    void IResourceSet::WriteStorageTexelBuffers(
        TexelBufferHandle *texelBuffers, const std::string &name, size_t startElement, size_t count
    )
    {
        for (size_t index = startElement; index < startElement + count; index++)
        {
            m_QueuedResources.StorageTexelBuffers[name][index] = *texelBuffers;
            texelBuffers++;
        }
    }

    const ResourceSetDescriptors &IResourceSet::GetBoundResources() const
    {
        return m_BoundResources;
    }
} // namespace Nexus::Graphics
