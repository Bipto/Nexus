#include <iostream>
#include "Core/Core.h"

int main(int, char**)
{
    NexusEngine::Init();
    std::cout << "Hello from NexusEngine!" << std::endl;

    NexusEngine::Shutdown();
    return 0;
}