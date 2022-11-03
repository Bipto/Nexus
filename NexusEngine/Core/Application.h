#pragma once

#include "Window.h"

namespace NexusEngine
{
    class Application
    {
        public:
            Application();
            Application(const Application&) = delete;
            ~Application();

            void Run();
        private:
            NexusEngine::Window* m_Window;
    };
}