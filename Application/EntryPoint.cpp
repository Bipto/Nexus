#include <iostream>

#include "Core/Core.h"
#include "Core/Application.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Window.h"

class Editor : public NexusEngine::Application
{
    public:
        Editor() : Application(NexusEngine::GraphicsAPI::OpenGL){}

        void Load() override
        {
            
        }

        void Update()
        {
            this->m_GraphicsDevice->SetContext();

            NexusEngine::WindowSize size = this->GetWindowSize();

            if (size.Height != this->m_PreviousSize.Height || size.Width != this->m_PreviousSize.Width)
            {
                this->m_GraphicsDevice->Resize(size.Width, size.Height);
            }

            this->m_GraphicsDevice->Clear(0.07f, 0.13f, 0.17f, 1);

            this->m_GraphicsDevice->SwapBuffers();

            this->m_PreviousSize = size;
        }

        void Unload()
        {
            
        }
    private:
        NexusEngine::WindowSize m_PreviousSize;
};

int main(int argc, char** argv)
{
    NexusEngine::Init();

    Editor* app = new Editor();
    NexusEngine::Run(app);
    delete app;

    NexusEngine::Shutdown();
    return 0;
} 