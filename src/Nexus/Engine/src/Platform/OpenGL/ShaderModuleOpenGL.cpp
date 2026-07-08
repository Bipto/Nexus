#if defined(NX_PLATFORM_OPENGL)

#include <regex>

#include "GraphicsDeviceOpenGL.hpp"
#include "OpenGLShaderParser.hpp"
#include "Platform/Logging/Log.hpp"
#include "ShaderModuleOpenGL.hpp"

namespace Nexus::Graphics
{
    ShaderModuleOpenGL::ShaderModuleOpenGL(
        const ShaderModuleDescription &shaderModuleSpec, GraphicsDeviceOpenGL *device
    )
        : IShaderModule(shaderModuleSpec), m_ShaderStage(GL::GetShaderStage(m_ModuleDescription.ShadingStage)),
          m_Device(device)
    {
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->Execute([&](const GladGLContext &context) {
            m_Handle = context.CreateShader(m_ShaderStage);
            const char *source = m_ModuleDescription.Source.c_str();
            glCall(context.ShaderSource(m_Handle, 1, &source, nullptr));
            glCall(context.CompileShader(m_Handle));

            int success;
            glCall(context.GetShaderiv(m_Handle, GL_COMPILE_STATUS, &success));

            if (!success)
            {
                char infoLog[512];
                glCall(context.GetShaderInfoLog(m_Handle, 512, NULL, infoLog));
                std::string errorMessage = "Error: Vertex Shader - " + std::string(infoLog);
                NX_ERROR(errorMessage);
            }

            if (context.KHR_debug)
            {
                context.ObjectLabelKHR(GL_SHADER, m_Handle, -1, m_ModuleDescription.DebugName.c_str());
            }
        });
    }

    ShaderModuleOpenGL::~ShaderModuleOpenGL()
    {
        GL::IGLContext *context = m_Device->GetOffscreenContext();
        context->Execute([&](const GladGLContext &context) { context.DeleteShader(m_Handle); });
    }

    GLenum ShaderModuleOpenGL::GetGLShaderStage() const
    {
        return m_ShaderStage;
    }

    uint32_t ShaderModuleOpenGL::GetHandle() const
    {
        return m_Handle;
    }

    static ReflectedShaderDataType StringToReflectedShaderDataType(
        const std::string &type, ResourceDimension &dimension, StorageResourceAccess &storageResourceAccess,
        const std::string &memoryQualifier
    )
    {
        dimension = ResourceDimension::NoDimension;
        storageResourceAccess = StorageResourceAccess::NoAccess;

        if (type == "float")
        {
            return ReflectedShaderDataType::Float;
        }
        else if (type == "vec2")
        {
            return ReflectedShaderDataType::Float2;
        }
        else if (type == "vec3")
        {
            return ReflectedShaderDataType::Float3;
        }
        else if (type == "vec4")
        {
            return ReflectedShaderDataType::Float4;
        }
        else if (type == "int")
        {
            return ReflectedShaderDataType::Int;
        }
        else if (type == "ivec2")
        {
            return ReflectedShaderDataType::Int2;
        }
        else if (type == "ivec3")
        {
            return ReflectedShaderDataType::Int3;
        }
        else if (type == "ivec4")
        {
            return ReflectedShaderDataType::Int4;
        }
        else if (type == "uint")
        {
            return ReflectedShaderDataType::UInt;
        }
        else if (type == "uvec2")
        {
            return ReflectedShaderDataType::UInt2;
        }
        else if (type == "uvec3")
        {
            return ReflectedShaderDataType::UInt3;
        }
        else if (type == "uvec4")
        {
            return ReflectedShaderDataType::UInt4;
        }
        else if (type == "bool")
        {
            return ReflectedShaderDataType::Boolean;
        }
        else if (type == "bvec2")
        {
            return ReflectedShaderDataType::Boolean2;
        }
        else if (type == "bvec3")
        {
            return ReflectedShaderDataType::Boolean3;
        }
        else if (type == "bvec4")
        {
            return ReflectedShaderDataType::Boolean4;
        }
        else if (type == "double")
        {
            return ReflectedShaderDataType::Double;
        }
        else if (type == "dvec2")
        {
            return ReflectedShaderDataType::Double2;
        }
        else if (type == "dvec3")
        {
            return ReflectedShaderDataType::Double3;
        }
        else if (type == "dvec4")
        {
            return ReflectedShaderDataType::Double4;
        }
        else if (type == "sampler1D" || type == "isampler1D" || type == "usampler1D")
        {
            dimension = ResourceDimension::Texture1D;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "sampler2D" || type == "isampler2D" || type == "usampler2D")
        {
            dimension = ResourceDimension::Texture2D;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "sampler3D" || type == "isampler3D" || type == "usampler3D")
        {
            dimension = ResourceDimension::Texture3D;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "samplerCube" || type == "isamplerCube" || type == "usamplerCube")
        {
            dimension = ResourceDimension::TextureCube;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "sampler2DRect" || type == "isampler2DRect" || type == "usampler2DRect")
        {
            dimension = ResourceDimension::TextureRectangle;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "sampler1DArray" || type == "isampler1DArray" || type == "usampler1DArray")
        {
            dimension = ResourceDimension::Texture1DArray;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "sampler2DArray" || type == "isampler2DArray" || type == "usampler2DArray")
        {
            dimension = ResourceDimension::Texture2DArray;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "samplerCubeArray" || type == "isamplerCubeArray" || type == "usamplerCubeArray")
        {
            dimension = ResourceDimension::TextureCubeArray;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "samplerBuffer" || type == "isamplerBuffer" || type == "usamplerBuffer")
        {
            dimension = ResourceDimension::NoDimension;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "sampler2DMS" || type == "isampler2DMS" || type == "usampler2DMS")
        {
            dimension = ResourceDimension::Texture2DMS;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "texture2DMSArray" || type == "imagee2DMSArray")
        {
            dimension = ResourceDimension::Texture2DMSArray;
            return ReflectedShaderDataType::CombinedImageSampler;
        }
        else if (type == "texture1D" || type == "image1D")
        {
            dimension = ResourceDimension::Texture1D;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "texture2D" || type == "image2D")
        {
            dimension = ResourceDimension::Texture2D;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "texture3D" || type == "image3D")
        {
            dimension = ResourceDimension::Texture3D;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "textureCube" || type == "imageCube")
        {
            dimension = ResourceDimension::TextureCube;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "texture2DRect" || type == "image2DRect")
        {
            dimension = ResourceDimension::TextureRectangle;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "texture1DArray" || type == "image1DArray")
        {
            dimension = ResourceDimension::Texture1DArray;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "texture2DArray" || type == "image2DArray")
        {
            dimension = ResourceDimension::Texture2DArray;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "textureCubeArray" || type == "imageCubeArray")
        {
            dimension = ResourceDimension::TextureCubeArray;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "textureBuffer" || type == "imageBuffer")
        {
            dimension = ResourceDimension::NoDimension;
            storageResourceAccess = StorageResourceAccess::Read;
            return ReflectedShaderDataType::UniformTextureBuffer;
        }
        else if (type == "samplerBuffer")
        {
            dimension = ResourceDimension::NoDimension;
            storageResourceAccess = StorageResourceAccess::ReadWrite;
            return ReflectedShaderDataType::StorageTextureBuffer;
        }
        else if (type == "texture2DMS" || type == "image2DMS")
        {
            dimension = ResourceDimension::Texture2DMS;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "texture2DMSArray" || type == "image2DMSArray")
        {
            dimension = ResourceDimension::Texture2DMSArray;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "sampler")
        {
            dimension = ResourceDimension::NoDimension;
            return ReflectedShaderDataType::Sampler;
        }
        else if (type == "sampler2DShadow")
        {
            dimension = ResourceDimension::Texture2D;
            return ReflectedShaderDataType::ComparisonSampler;
        }
        else if (type == "samplerCubeShadow")
        {
            dimension = ResourceDimension::TextureCube;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "sampler2DArrayShadow")
        {
            dimension = ResourceDimension::Texture2DArray;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "samplerCubeArrayShadow")
        {
            dimension = ResourceDimension::TextureCubeArray;
            return ReflectedShaderDataType::Texture;
        }
        else if (type == "mat2")
        {
            return ReflectedShaderDataType::Mat2x2;
        }
        else if (type == "mat2x3")
        {
            return ReflectedShaderDataType::Mat2x3;
        }
        else if (type == "mat2x4")
        {
            return ReflectedShaderDataType::Mat2x4;
        }
        else if (type == "mat3x2")
        {
            return ReflectedShaderDataType::Mat3x2;
        }
        else if (type == "mat3")
        {
            return ReflectedShaderDataType::Mat3x3;
        }
        else if (type == "mat3x4")
        {
            return ReflectedShaderDataType::Mat3x4;
        }
        else if (type == "mat4x2")
        {
            return ReflectedShaderDataType::Mat4x2;
        }
        else if (type == "mat4x3")
        {
            return ReflectedShaderDataType::Mat4x3;
        }
        else if (type == "mat4")
        {
            return ReflectedShaderDataType::Mat4x4;
        }
        else if (type == "dmat2")
        {
            return ReflectedShaderDataType::DMat2x2;
        }
        else if (type == "dmat2x3")
        {
            return ReflectedShaderDataType::DMat2x3;
        }
        else if (type == "dmat2x4")
        {
            return ReflectedShaderDataType::DMat2x4;
        }
        else if (type == "dmat3x2")
        {
            return ReflectedShaderDataType::DMat3x2;
        }
        else if (type == "dmat3")
        {
            return ReflectedShaderDataType::DMat3x3;
        }
        else if (type == "dmat3x4")
        {
            return ReflectedShaderDataType::DMat3x4;
        }
        else if (type == "dmat4x2")
        {
            return ReflectedShaderDataType::DMat4x2;
        }
        else if (type == "dmat4x3")
        {
            return ReflectedShaderDataType::DMat4x3;
        }
        else if (type == "dmat4")
        {
            return ReflectedShaderDataType::DMat4x4;
        }
        else if (type == "buffer")
        {
            if (memoryQualifier == "readonly")
            {
                storageResourceAccess = StorageResourceAccess::Read;
            }
            else if (memoryQualifier == "writeonly")
            {
                storageResourceAccess = StorageResourceAccess::Write;
            }
            else
            {
                storageResourceAccess = StorageResourceAccess::ReadWrite;
            }

            return ReflectedShaderDataType::StorageBuffer;
        }
        else if (type == "uniform")
        {
            return ReflectedShaderDataType::UniformBuffer;
        }
        else if (type == "shared")
        {
            return ReflectedShaderDataType::Shared;
        }

        throw std::runtime_error("Failed to find a valid type");
    }

    static std::optional<uint32_t> ExtractUniformBindingLocation(const std::string &input)
    {
        std::regex pattern(R"(location\s*=\s*(\d+))");

        std::smatch match;
        if (std::regex_search(input, match, pattern))
        {
            uint32_t number = (uint32_t)std::stoi(match[1]);
            return number;
        }

        return {};
    }

    static std::optional<uint32_t> ExtractBufferBindingLocation(const std::string &input)
    {
        std::regex pattern(R"(binding\s*=\s*(\d+))");

        std::smatch match;
        if (std::regex_search(input, match, pattern))
        {
            uint32_t number = (uint32_t)std::stoi(match[1]);
            return number;
        }

        return {};
    }

    static ReflectedResource ExtractResourceType(const OpenGL::ReflectedShaderResource &uniform)
    {
        ResourceDimension dimension = {};
        StorageResourceAccess storageResourceAccess = {};
        ReflectedShaderDataType type =
            StringToReflectedShaderDataType(uniform.Type, dimension, storageResourceAccess, uniform.MemoryQualififers);

        ReflectedResource reflectedResource = {};
        reflectedResource.Type = type;
        reflectedResource.Dimension = dimension;
        reflectedResource.ResourceAccess = storageResourceAccess;
        reflectedResource.InstanceName = uniform.Name;

        std::optional<uint32_t> bindingPoint = ExtractUniformBindingLocation(uniform.LayoutQualififers);
        if (bindingPoint)
        {
            reflectedResource.BindingPoint = bindingPoint.value();
        }
        else
        {
            reflectedResource.BindingPoint = 0;
        }

        std::optional<uint32_t> arraySize = uniform.ArraySize;
        if (arraySize)
        {
            reflectedResource.BindingCount = arraySize.value();
        }
        else
        {
            reflectedResource.BindingCount = 1;
        }

        return reflectedResource;
    }

    static ReflectedResource ExtractBuffer(const OpenGL::ReflectedShaderBuffer &reflectedBuffer)
    {
        ResourceDimension dimension = {};
        StorageResourceAccess storageResourceAccess = {};
        ReflectedShaderDataType type = StringToReflectedShaderDataType(
            reflectedBuffer.StorageQualifier, dimension, storageResourceAccess, reflectedBuffer.MemoryQualififers
        );

        ReflectedResource reflectedResource = {};
        reflectedResource.Type = type;
        reflectedResource.Dimension = dimension;
        reflectedResource.ResourceAccess = storageResourceAccess;
        reflectedResource.InstanceName = reflectedBuffer.InstanceName;
        reflectedResource.BlockName = reflectedBuffer.BlockName;

        std::optional<uint32_t> bindingPoint = ExtractBufferBindingLocation(reflectedBuffer.LayoutQualifiers);
        if (bindingPoint)
        {
            reflectedResource.BindingPoint = bindingPoint.value();
        }
        else
        {
            reflectedResource.BindingPoint = 0;
        }

        reflectedResource.BindingCount = 1;

        return reflectedResource;
    }

    static ShaderReflectionData ExtractReflectionData(const OpenGL::ReflectedShaderResources &data)
    {
        ShaderReflectionData reflectionData = {};

        for (const OpenGL::ReflectedShaderResource &resource : data.Uniforms)
        {
            // inputs
            if (resource.StorageQualifier == "in" || resource.StorageQualifier == "attribute")
            {
                Graphics::ReflectedResource reflectedResource = ExtractResourceType(resource);

                Attribute &attribute = reflectionData.Inputs.emplace_back();
                attribute.Name = reflectedResource.InstanceName;
                attribute.Binding = reflectedResource.BindingPoint;
                attribute.Type = reflectedResource.Type;
            }
            // outputs
            else if (resource.StorageQualifier == "out")
            {
                Graphics::ReflectedResource reflectedResource = ExtractResourceType(resource);

                Attribute &attribute = reflectionData.Outputs.emplace_back();
                attribute.Name = reflectedResource.InstanceName;
                attribute.Binding = reflectedResource.BindingPoint;
                attribute.Type = reflectedResource.Type;
            }
            // for regular uniforms (buffers and textures/samplers etc...)
            else
            {
                Graphics::ReflectedResource reflectedResource = ExtractResourceType(resource);
                reflectionData.Resources.push_back(reflectedResource);
            }
        }

        for (const auto &buffer : data.Buffers)
        {
            Graphics::ReflectedResource reflectedBuffer = ExtractBuffer(buffer);
            reflectionData.Resources.push_back(reflectedBuffer);
        }

        return reflectionData;
    }

    ShaderReflectionData ShaderModuleOpenGL::Reflect() const
    {
        OpenGL::OpenGLShaderParser shaderParser;
        OpenGL::ReflectedShaderResources resources = shaderParser.ReflectShader(m_ModuleDescription.Source);
        return ExtractReflectionData(resources);
    }
} // namespace Nexus::Graphics

#endif