#pragma once

#include <iostream>
#include "SDL.h"

#include <Python.h>

namespace NexusEngine
{
    static void Init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "Could not initialize SDL\n";
        }

        Py_Initialize();
        PyRun_SimpleString("print('Hello from Python')");
    }
    static void Shutdown()
    {
        SDL_Quit();
        Py_Finalize();
    }
}