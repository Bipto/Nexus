#pragma once

#include "Core/Graphics/GraphicsDevice.h"
#include "GL.h"
#include "SDL_opengl.h"
#include "ShaderOpenGL.h"
#include "BufferOpenGL.h"
#include "TextureOpenGL.h"
#include "FramebufferOpenGL.h"

namespace Nexus
{
    class GraphicsDeviceOpenGL : public GraphicsDevice
    {
        public:
            GraphicsDeviceOpenGL(Nexus::Window* window, GraphicsAPI api);
            GraphicsDeviceOpenGL(const GraphicsDeviceOpenGL&) = delete;
            virtual ~GraphicsDeviceOpenGL();
            void SetContext() override;
            void Clear(float red, float green, float blue, float alpha) override;
            void DrawElements(Ref<VertexBuffer> vertexBuffer, Ref<Shader> shader) override;
            void DrawIndexed(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, Ref<Shader> shader) override;

            virtual const char* GetAPIName() override;
            virtual const char* GetDeviceName() override;
            virtual void* GetContext() override;

            virtual Ref<Shader> CreateShaderFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const BufferLayout& layout) override;
            virtual Ref<Shader> CreateShaderFromFile(const std::string& filepath, const BufferLayout& layout) override;
            virtual Ref<VertexBuffer> CreateVertexBuffer(const std::vector<float> vertices) override;
            virtual Ref<IndexBuffer> CreateIndexBuffer(const std::vector<unsigned int> indices) override;
            virtual Ref<Texture> CreateTexture(const char* filepath) override;
            virtual Ref<Framebuffer> CreateFramebuffer(const Nexus::FramebufferSpecification& spec) override;
            
            virtual void InitialiseImGui() override;
            virtual void BeginImGuiRender() override;
            virtual void EndImGuiRender() override;

            virtual void Resize(Point size) override;
            virtual void SwapBuffers() override;
            virtual void SetVSyncState(VSyncState vSyncState) override;

        private:
            SDL_GLContext m_Context;
            const char* m_GlslVersion;
    };
}