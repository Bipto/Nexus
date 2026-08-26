#if defined(NX_PLATFORM_OPENGL)

#include "ResourceSetOpenGL.hpp"
#include "PipelineOpenGL.hpp"
#include "TexelBufferOpenGL.hpp"
#include "TextureViewOpenGL.hpp"

namespace Nexus::Graphics
{
    ResourceSetOpenGL::ResourceSetOpenGL(PipelineHandle pipeline, GraphicsDeviceOpenGL *device) : IResourceSet(pipeline)
    {
        const ResourceSetDescription &resourceSetDesc = pipeline->GetResourceSetDescription();

        GL::IOffscreenContext *context = device->GetOffscreenContext();
        const PipelineOpenGL *pipelineGL = pipeline.AsDerived<const PipelineOpenGL>();

        for (const ResourceDescriptor &descriptor : resourceSetDesc.Descriptors)
        {
            // check whether the resource is a buffer, as their locations have to be
            // retrieved differently
            bool isUniformBuffer = descriptor.Type == ResourceDescriptorType::UniformBuffer ||
                                   descriptor.Type == ResourceDescriptorType::DynamicUniformBuffer ||
                                   descriptor.Type == ResourceDescriptorType::InlineUniformBlock ||
                                   descriptor.Type == ResourceDescriptorType::PushConstants;

            bool isStorageBuffer = descriptor.Type == ResourceDescriptorType::StorageBuffer ||
                                   descriptor.Type == ResourceDescriptorType::DynamicStorageBuffer;

            if (isUniformBuffer)
            {
                if (descriptor.CountOrSizeInBytes == 1 || descriptor.Type == ResourceDescriptorType::PushConstants ||
                    descriptor.Type == ResourceDescriptorType::InlineUniformBlock)
                {
                    int32_t location =
                        context->GetUniformBlockIndex(pipelineGL->GetShaderHandle(), descriptor.Name.c_str());
                    m_BindingLocations[descriptor.Name] = {location};
                }
                else
                {
                    for (size_t i = 0; i < descriptor.CountOrSizeInBytes; i++)
                    {
                        std::stringstream ss;
                        ss << descriptor.Name << "[" << std::to_string(i) << "]";

                        int32_t location = context->GetUniformLocation(pipelineGL->GetShaderHandle(), ss.str().c_str());
                        m_BindingLocations[descriptor.Name].push_back(location);
                    }
                }
            }
            else if (isStorageBuffer)
            {
                if (context->AreStorageBuffersSupported())
                {
                    if (descriptor.CountOrSizeInBytes == 1)
                    {
                        int32_t location = context->GetProgramResourceIndex(
                            pipelineGL->GetShaderHandle(), GL_SHADER_STORAGE_BLOCK, descriptor.Name.c_str());
                        m_BindingLocations[descriptor.Name] = {location};
                    }
                    else
                    {
                        for (size_t i = 0; i < descriptor.CountOrSizeInBytes; i++)
                        {
                            std::stringstream ss;
                            ss << descriptor.Name << "[" << std::to_string(i) << "]";

                            int32_t location = context->GetProgramResourceIndex(
                                pipelineGL->GetShaderHandle(), GL_SHADER_STORAGE_BLOCK, ss.str().c_str());
                            m_BindingLocations[descriptor.Name].push_back(location);
                        }
                    }
                }
            }
            else
            {
                if (descriptor.CountOrSizeInBytes == 1)
                {
                    int32_t location =
                        context->GetUniformLocation(pipelineGL->GetShaderHandle(), descriptor.Name.c_str());
                    m_BindingLocations[descriptor.Name] = {location};
                }
                else
                {
                    for (size_t i = 0; i < descriptor.CountOrSizeInBytes; i++)
                    {
                        std::stringstream ss;
                        ss << descriptor.Name << "[" << std::to_string(i) << "]";

                        int32_t location = context->GetUniformLocation(pipelineGL->GetShaderHandle(), ss.str().c_str());
                        m_BindingLocations[descriptor.Name].push_back(location);
                    }
                }
            }

            // create emulated resources
            if (descriptor.Type == ResourceDescriptorType::PushConstants)
            {
                DeviceBufferDescription bufferDesc = {};
                bufferDesc.Usage = BufferUsage_Uniform;
                bufferDesc.DebugName = descriptor.Name;
                bufferDesc.Access = BufferMemoryAccess::Upload;
                bufferDesc.SizeInBytes = descriptor.CountOrSizeInBytes;
                bufferDesc.StrideInBytes = descriptor.CountOrSizeInBytes;

                m_EmulatedPushConstants[descriptor.Name] = CreateRef<DeviceBufferOpenGL>(bufferDesc, device);
            }
            else if (descriptor.Type == ResourceDescriptorType::InlineUniformBlock)
            {
                DeviceBufferDescription bufferDesc = {};
                bufferDesc.Usage = BufferUsage_Uniform;
                bufferDesc.DebugName = descriptor.Name;
                bufferDesc.Access = BufferMemoryAccess::Upload;
                bufferDesc.SizeInBytes = descriptor.CountOrSizeInBytes;
                bufferDesc.StrideInBytes = descriptor.CountOrSizeInBytes;

                m_EmulatedInlineUniformBlocks[descriptor.Name] = CreateRef<DeviceBufferOpenGL>(bufferDesc, device);
            }
        }

        // iterate through all immutable samplers and get their locations
        for (const auto &[name, samplers] : resourceSetDesc.ImmutableSamplers)
        {
            if (samplers.size() == 1)
            {
                int32_t location = context->GetUniformLocation(pipelineGL->GetShaderHandle(), name.c_str());
                m_ImmutableSamplers[location] = samplers[0].AsDerived<const SamplerOpenGL>();
            }
            else
            {
                for (size_t i = 0; i < samplers.size(); i++)
                {
                    std::stringstream ss;
                    ss << name << "[" << std::to_string(i) << "]";

                    int32_t location = context->GetUniformLocation(pipelineGL->GetShaderHandle(), ss.str().c_str());
                    m_ImmutableSamplers[location] = samplers[i].AsDerived<const SamplerOpenGL>();
                }
            }
        }
    }

    void ResourceSetOpenGL::Flush()
    {
        // uniform buffers
        for (const auto &[name, views] : m_QueuedResources.UniformBuffers)
        {
            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];
                if (const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>())
                {
                    m_BoundResources.UniformBuffers[name][arrayIndex] = view;
                }
            }
        }

        // dynamic uniform buffers
        for (const auto &[name, views] : m_QueuedResources.DynamicUniformBuffers)
        {
            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];
                if (const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>())
                {
                    m_BoundResources.DynamicUniformBuffers[name][arrayIndex] = view;
                }
            }
        }

        // inline uniform block
        for (const auto &[name, inlineData] : m_QueuedResources.InlineUniformBlocks)
        {
            m_BoundResources.InlineUniformBlocks[name] = inlineData;
            m_EmulatedInlineUniformBlocks[name]->SetData(inlineData.data(), 0, inlineData.size());
        }

        // storage buffers
        for (const auto &[name, views] : m_QueuedResources.StorageBuffers)
        {
            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];
                if (const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>())
                {
                    m_BoundResources.StorageBuffers[name][arrayIndex] = view;
                }
            }
        }

        // dynamic storage buffers
        for (const auto &[name, views] : m_QueuedResources.DynamicStorageBuffers)
        {
            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];
                if (const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>())
                {
                    m_BoundResources.DynamicStorageBuffers[name][arrayIndex] = view;
                }
            }
        }

        // storage images
        for (const auto &[name, storageImages] : m_QueuedResources.StorageImages)
        {
            for (size_t arrayIndex = 0; arrayIndex < storageImages.size(); arrayIndex++)
            {
                const auto &storageImage = storageImages[arrayIndex];
                if (storageImage.Texture.IsValid())
                {
                    m_BoundResources.StorageImages[name][arrayIndex] = storageImage;
                }
            }
        }

        // combined image samplers
        for (const auto &[name, combinedImageSamplers] : m_QueuedResources.CombinedImageSamplers)
        {
            for (size_t arrayIndex = 0; arrayIndex < combinedImageSamplers.size(); arrayIndex++)
            {
                const auto &combinedImageSampler = combinedImageSamplers[arrayIndex];

                const TextureViewOpenGL *textureView =
                    combinedImageSampler.ImageTexture.AsDerived<const TextureViewOpenGL>();
                if (textureView && combinedImageSampler.ImageSampler.IsValid())
                {
                    m_BoundResources.CombinedImageSamplers[name][arrayIndex] = combinedImageSampler;
                }
            }
        }

        // sampled images
        for (const auto &[name, sampledImages] : m_QueuedResources.SampledImages)
        {
            for (size_t arrayIndex = 0; arrayIndex < sampledImages.size(); arrayIndex++)
            {
                const auto &sampledImage = sampledImages[arrayIndex];

                if (const TextureViewOpenGL *textureView = sampledImage.AsDerived<const TextureViewOpenGL>())
                {
                    m_BoundResources.SampledImages[name][arrayIndex] = sampledImage;
                }
            }
        }

        // samplers
        for (const auto &[name, samplers] : m_QueuedResources.Samplers)
        {
            for (size_t arrayIndex = 0; arrayIndex < samplers.size(); arrayIndex++)
            {
                const auto &sampler = samplers[arrayIndex];

                if (sampler.IsValid())
                {
                    m_BoundResources.Samplers[name][arrayIndex] = sampler;
                }
            }
        }

        // uniform texel buffers
        for (const auto &[name, texelBuffers] : m_QueuedResources.UniformTexelBuffers)
        {
            for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
            {
                const auto &texelBuffer = texelBuffers[arrayIndex];
                if (const TexelBufferOpenGL *texelBufferVk = texelBuffer.AsDerived<const TexelBufferOpenGL>())
                {
                    m_BoundResources.UniformTexelBuffers[name][arrayIndex] = texelBuffer;
                }
            }
        }

        // storage texel buffers
        for (const auto &[name, texelBuffers] : m_QueuedResources.StorageTexelBuffers)
        {
            for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
            {
                const auto &texelBuffer = texelBuffers[arrayIndex];
                if (const TexelBufferOpenGL *texelBufferVk = texelBuffer.AsDerived<const TexelBufferOpenGL>())
                {
                    m_BoundResources.UniformTexelBuffers[name][arrayIndex] = texelBuffer;
                }
            }
        }

        // immutable samplers
        for (const auto &[location, sampler] : m_ImmutableSamplers)
        {
            sampler->Bind(location);
        }
    }

    void ResourceSetOpenGL::Bind(const ResourceSetBindingDescription &bindingDesc, uint32_t programHandle,
                                 GL::IOffscreenContext *context)
    {
        uint32_t uniformBufferBindingPoint = 0;
        uint32_t storageBufferBindingPoint = 0;

        // pusg constants
        for (const auto &[name, buffer] : m_EmulatedPushConstants)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);
            int32_t bindingIndex = bindingPoints[0];
            if (bindingIndex != -1)
            {
                context->UniformBlockBinding(programHandle, bindingIndex, uniformBufferBindingPoint);
                context->BindBufferRange(GL_UNIFORM_BUFFER, uniformBufferBindingPoint, buffer->GetHandle(), 0,
                                         buffer->GetSizeInBytes());
                uniformBufferBindingPoint++;
            }
        }

        // uniform buffers
        for (const auto &[name, views] : m_BoundResources.UniformBuffers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);
            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];

                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>();

                if (buffer && bindingIndex != -1)
                {
                    context->UniformBlockBinding(programHandle, bindingIndex, uniformBufferBindingPoint);
                    context->BindBufferRange(GL_UNIFORM_BUFFER, uniformBufferBindingPoint, buffer->GetHandle(),
                                             view.Offset, view.Size);
                    uniformBufferBindingPoint++;
                }
            }
        }

        // dynamic uniform buffers
        for (const auto &[name, views] : m_BoundResources.DynamicUniformBuffers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);
            const std::vector<uint32_t> &dynamicOffsets = bindingDesc.DynamicOffsets.at(name);

            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];

                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>();

                uint32_t dynamicOffset = 0;
                if (arrayIndex < dynamicOffsets.size())
                {
                    dynamicOffset = dynamicOffsets[arrayIndex];
                }

                if (buffer && bindingIndex != -1)
                {
                    context->UniformBlockBinding(programHandle, bindingIndex, uniformBufferBindingPoint);
                    context->BindBufferRange(GL_UNIFORM_BUFFER, uniformBufferBindingPoint, buffer->GetHandle(),
                                             dynamicOffset, view.Size);
                    uniformBufferBindingPoint++;
                }
            }
        }
        // inline uniform block
        for (const auto &[name, uniformBuffer] : m_EmulatedInlineUniformBlocks)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);
            int32_t bindingIndex = bindingPoints[0];
            if (bindingIndex != -1)
            {
                context->UniformBlockBinding(programHandle, bindingIndex, uniformBufferBindingPoint);
                context->BindBufferRange(GL_UNIFORM_BUFFER, uniformBufferBindingPoint, uniformBuffer->GetHandle(), 0,
                                         uniformBuffer->GetSizeInBytes());
                uniformBufferBindingPoint++;
            }
        }

        // storage buffers
        for (const auto &[name, views] : m_BoundResources.StorageBuffers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);
            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];

                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>();

                if (buffer && bindingIndex != -1)
                {
                    context->ShaderStorageBlockBinding(programHandle, bindingIndex, storageBufferBindingPoint);
                    context->BindBufferRange(GL_SHADER_STORAGE_BUFFER, storageBufferBindingPoint, buffer->GetHandle(),
                                             view.Offset, view.SizeInBytes);
                    storageBufferBindingPoint++;
                }
            }
        }

        // dynamic storage buffers
        for (const auto &[name, views] : m_BoundResources.DynamicStorageBuffers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);
            const std::vector<uint32_t> &dynamicOffsets = bindingDesc.DynamicOffsets.at(name);

            for (size_t arrayIndex = 0; arrayIndex < views.size(); arrayIndex++)
            {
                const auto &view = views[arrayIndex];
                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const DeviceBufferOpenGL *buffer = view.BufferHandle.AsDerived<const DeviceBufferOpenGL>();

                uint32_t dynamicOffset = 0;
                if (arrayIndex < dynamicOffsets.size())
                {
                    dynamicOffset = dynamicOffsets[arrayIndex];
                }

                if (buffer && bindingIndex != -1)
                {
                    context->ShaderStorageBlockBinding(programHandle, bindingIndex, storageBufferBindingPoint);
                    context->BindBufferRange(GL_SHADER_STORAGE_BUFFER, storageBufferBindingPoint, buffer->GetHandle(),
                                             dynamicOffset, view.SizeInBytes);
                    storageBufferBindingPoint++;
                }
            }
        }

        // storage images
        for (const auto &[name, storageImages] : m_BoundResources.StorageImages)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);
            for (size_t arrayIndex = 0; arrayIndex < storageImages.size(); arrayIndex++)
            {
                const auto &storageImage = storageImages[arrayIndex];
                int32_t bindingIndex = bindingPoints.at(arrayIndex);

                TextureHandle textureHandle = storageImage.Texture;
                TextureOpenGL *texture = textureHandle.AsDerived<TextureOpenGL>();

                if (texture && bindingIndex != -1)
                {
                    GLenum format = GL::GetSizedInternalFormat(texture->GetDescription().Format);
                    GLenum access = GL::GetAccessMask(storageImage.Access);
                    bool isLayered = storageImage.ArrayLayer != 0;

                    context->BindImageTexture(bindingIndex, texture->GetHandle(), storageImage.MipLevel,
                                              isLayered ? GL_TRUE : GL_FALSE, storageImage.ArrayLayer, access, format);

                    if (storageImage.Access == ShaderAccess::ReadWrite)
                    {
                        texture->MarkDirty();
                    }
                }
            }
        }

        // combined image samplers
        for (const auto &[name, combinedImageSamplers] : m_BoundResources.CombinedImageSamplers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);

            for (size_t arrayIndex = 0; arrayIndex < combinedImageSamplers.size(); arrayIndex++)
            {
                const auto &combinedImageSampler = combinedImageSamplers[arrayIndex];
                int32_t bindingIndex = bindingPoints.at(arrayIndex);

                const TextureViewOpenGL *textureView =
                    combinedImageSampler.ImageTexture.AsDerived<const TextureViewOpenGL>();
                const SamplerOpenGL *sampler = combinedImageSampler.ImageSampler.AsDerived<const SamplerOpenGL>();

                if (textureView && sampler && bindingIndex != -1)
                {
                    const TextureViewDescription &viewDesc = textureView->GetDescription();
                    textureView->Bind(bindingIndex);
                    sampler->Bind(bindingIndex);
                }
            }
        }

        // sampled images
        for (const auto &[name, sampledImages] : m_BoundResources.SampledImages)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);

            for (size_t arrayIndex = 0; arrayIndex < sampledImages.size(); arrayIndex++)
            {
                const auto &sampledImage = sampledImages[arrayIndex];
                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const TextureViewOpenGL *textureView = sampledImage.AsDerived<const TextureViewOpenGL>();

                if (textureView && bindingIndex != -1)
                {
                    textureView->Bind(bindingIndex);
                }
            }
        }

        // samplers
        for (const auto &[name, samplers] : m_BoundResources.Samplers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);

            for (size_t arrayIndex = 0; arrayIndex < samplers.size(); arrayIndex++)
            {
                const auto &sampler = samplers[arrayIndex];
                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const SamplerOpenGL *samplerGL = sampler.AsDerived<const SamplerOpenGL>();

                if (samplerGL && bindingIndex != -1)
                {
                    samplerGL->Bind(bindingIndex);
                }
            }
        }

        // uniform texel buffers
        for (const auto &[name, texelBuffers] : m_BoundResources.UniformTexelBuffers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);

            for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
            {
                const auto &texelBuffer = texelBuffers[arrayIndex];
                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const TexelBufferOpenGL *texelBufferGL = texelBuffer.AsDerived<const TexelBufferOpenGL>();

                if (texelBufferGL && bindingIndex != -1)
                {
                    texelBufferGL->Bind(bindingIndex);
                }
            }
        }

        // storage texel buffers
        for (const auto &[name, texelBuffers] : m_BoundResources.StorageTexelBuffers)
        {
            const std::vector<int32_t> &bindingPoints = m_BindingLocations.at(name);

            for (size_t arrayIndex = 0; arrayIndex < texelBuffers.size(); arrayIndex++)
            {
                const auto &texelBuffer = texelBuffers[arrayIndex];
                int32_t bindingIndex = bindingPoints.at(arrayIndex);
                const TexelBufferOpenGL *texelBufferGL = texelBuffer.AsDerived<const TexelBufferOpenGL>();

                if (texelBufferGL && bindingIndex != -1)
                {
                    texelBufferGL->Bind(bindingIndex);
                }
            }
        }

        // set up iummutable samplers
        if (const IPipeline *pipeline = m_Pipeline.AsDerived<const IPipeline>())
        {
            const ResourceSetDescription &resourceSetDesc = pipeline->GetResourceSetDescription();
        }
    }

    void ResourceSetOpenGL::SetPushConstants(const std::string &name, const void *data, size_t offset, size_t size)
    {
        if (m_EmulatedPushConstants.contains(name))
        {
            m_EmulatedPushConstants[name]->SetData(data, offset, size);
        }
    }

} // namespace Nexus::Graphics

#endif