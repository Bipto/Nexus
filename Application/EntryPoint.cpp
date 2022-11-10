#include <iostream>

#include "Core/Core.h"
#include "Core/Application.h"
#include "Core/Graphics/GraphicsDevice.h"
#include "Core/Window.h"
#include "Core/Size.h"
#include "Core/Graphics/Swapchain.h"

class Editor : public NexusEngine::Application
{
    public:
        Editor() : Application(NexusEngine::GraphicsAPI::OpenGL){}

        void Load() override
        {
            this->m_GraphicsDevice->GetSwapchain()->SetVSyncState(NexusEngine::VSyncState::Disabled);
        }

        void Update()
        {
            this->m_GraphicsDevice->SetContext();
            NexusEngine::Size size = this->GetWindowSize();

            if (size.Height != this->m_PreviousSize.Height || size.Width != this->m_PreviousSize.Width)
            {
                this->m_GraphicsDevice->GetSwapchain()->Resize(size);
            }

            this->m_GraphicsDevice->Clear(0.07f, 0.13f, 0.17f, 1);
            this->m_GraphicsDevice->GetSwapchain()->Present();

            this->m_PreviousSize = size;
        }

        void Unload()
        {
            
        }
    private:
        NexusEngine::Size m_PreviousSize;
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