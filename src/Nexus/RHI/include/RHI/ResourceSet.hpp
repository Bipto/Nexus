#pragma once

#include "RHI/AccelerationStructure.hpp"
#include "RHI/ResourceDeclarations.hpp"
#include "RHI/ResourceSetDescription.hpp"
#include "RHI/ShaderResources.hpp"
#include "RHI/TexelBuffer.hpp"

namespace Nexus::Graphics
{

    using InlineBlock = std::vector<uint8_t>;

    struct NX_RHI_API ResourceSetDescriptors
    {
        std::map<std::string, std::vector<UniformBufferView>> UniformBuffers = {};
        std::map<std::string, std::vector<UniformBufferView>> DynamicUniformBuffers =
            {};
        std::map<std::string, InlineBlock> InlineUniformBlocks = {};
        std::map<std::string, std::vector<StorageBufferView>> StorageBuffers = {};
        std::map<std::string, std::vector<StorageBufferView>> DynamicStorageBuffers =
            {};
        std::map<std::string, std::vector<StorageImageView>> StorageImages = {};
        std::map<std::string, std::vector<CombinedImageSampler>>
            CombinedImageSamplers = {};
        std::map<std::string, std::vector<TextureViewHandle>> SampledImages = {};
        std::map<std::string, std::vector<Graphics::SamplerHandle>> Samplers = {};
        std::map<std::string, std::vector<AccelerationStructureHandle>>
            AccelerationStructures = {};
        std::map<std::string, std::vector<TexelBufferHandle>> UniformTexelBuffers =
            {};
        std::map<std::string, std::vector<TexelBufferHandle>> StorageTexelBuffers =
            {};
    };

    class NX_RHI_API IResourceSet
    {
      public:
        IResourceSet(PipelineHandle pipeline);
        virtual ~IResourceSet();

        // single descriptors
        void WriteUniformBuffer(
            const UniformBufferView &uniformBuffers, const std::string &name
        );
        void WriteDynamicUniformBuffer(
            const UniformBufferView &uniformBuffers, const std::string &name
        );
        void WriteInlineUniformBlock(
            const void *data, size_t sizeInBytes, const std::string &name
        );
        void WriteStorageBuffer(
            const StorageBufferView &views, const std::string &name
        );
        void WriteDynamicStorageBuffer(
            const StorageBufferView &storageBuffers, const std::string &name
        );
        void WriteStorageImage(
            const StorageImageView &views, const std::string &name
        );
        void WriteCombinedImageSampler(
            const CombinedImageSampler &combinedImageSamplers,
            const std::string &name
        );
        void WriteSampledImage(
            TextureViewHandle textureView, const std::string &name
        );
        void WriteSampler(SamplerHandle sampler, const std::string &name);
        void WriteAccelerationStructure(
            AccelerationStructureHandle accelerationStructure,
            const std::string &name
        );
        void WriteUniformTexelBuffer(
            TexelBufferHandle texelBuffer, const std::string &name
        );
        void WriteStorageTexelBuffer(
            TexelBufferHandle texelBuffer, const std::string &name
        );

        // arrays
        void WriteUniformBuffers(
            const UniformBufferView *uniformBuffers, const std::string &name,
            size_t startElement, size_t count
        );
        void WriteDynamicUniformBuffers(
            const UniformBufferView *uniformBuffers, const std::string &name,
            size_t startElement, size_t count
        );
        void WriteStorageBuffers(
            const StorageBufferView *views, const std::string &name,
            size_t startElement, size_t count
        );
        void WriteDynamicStorageBuffers(
            const StorageBufferView *storageBuffers, const std::string &name,
            size_t startElement, size_t count
        );
        void WriteStorageImages(
            const StorageImageView *views, const std::string &name,
            size_t startElement, size_t count
        );
        void WriteCombinedImageSamplers(
            const CombinedImageSampler *combinedImageSamplers,
            const std::string &name, size_t startElement, size_t count
        );
        void WriteSampledImages(
            TextureViewHandle *textureViews, const std::string &name,
            size_t startElement, size_t count
        );
        void WriteSamplers(
            SamplerHandle *samplers, const std::string &name, size_t startElement,
            size_t count
        );
        void WriteAccelerationStructures(
            AccelerationStructureHandle *accelerationStructures,
            const std::string &name, size_t startElement, size_t count
        );
        void WriteUniformTexelBuffers(
            TexelBufferHandle *texelBuffers, const std::string &name,
            size_t startElement, size_t count
        );
        void WriteStorageTexelBuffers(
            TexelBufferHandle *texelBuffers, const std::string &name,
            size_t startElement, size_t count
        );

        virtual void Flush() = 0;

        const ResourceSetDescriptors &GetBoundResources() const;

      protected:
        PipelineHandle m_Pipeline = {};

        std::map<std::string, Nexus::Graphics::ShaderResource> m_ShaderResources;

        ResourceSetDescriptors m_BoundResources = {};
        ResourceSetDescriptors m_QueuedResources = {};
    };
} // namespace Nexus::Graphics