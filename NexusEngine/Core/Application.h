#pragma once

namespace NexusEngine
{
    enum WindowState
    {
        Normal,
        Minimized,
        Maximized
    };

    struct WindowSize
    {
        int Width, Height;
    };

    class Application
    {
        public:
            Application();
    };
}