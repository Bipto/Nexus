#include "NexusEngine.h"

class Demo : public Nexus::Application
{
    public:
        Demo(Nexus::GraphicsAPI api) : Application(api){}

        virtual void Load() override
        {

        }

        virtual void Update(Nexus::Time time) override
        {
            m_GraphicsDevice->Clear(0.8f, 0.2f, 0.3f, 1.0f);
            m_GraphicsDevice->SwapBuffers();   
        }

        virtual void OnResize(Nexus::Point size) override
        {
            m_GraphicsDevice->Resize(size);
        }

        virtual void Unload() override
        {

        }

        virtual bool OnClose() override 
        {
            return true;
        }
};

int main(int argc, char** argv)
{
    Nexus::Init();

    Demo* app = new Demo(Nexus::GraphicsAPI::OpenGL);
    Nexus::Run(app);
    delete app;

    Nexus::Shutdown();
    return 0;
}